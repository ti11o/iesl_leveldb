#include "CLevel.h"
#include "LevelDB.h"


//extern std::string FLAGS_op;

extern std::string FLAGS_db;

extern std::string FLAGS_dir;

extern std::string FLAGS_key;

extern std::string FLAGS_json;



leveldb::DB* db;
leveldb::WriteOptions wopt;
leveldb::ReadOptions ropt;
leveldb::WriteBatch batch;

CLevel::CLevel(){}

std::string generateKey(int seq){
	std::string key = std::to_string(seq);
	int l = key.length();
	for(int i = l; i < 16; i++){
		key.insert(0, 1, '0');
	}
	return key;
}

std::vector<std::string> split(std::string s, std::string delim){
    size_t pos_start = 0, pos_end, delim_len = delim.length();
    std::string token;
    std::vector<std::string> result;

    while((pos_end = s.find(delim, pos_start)) != std::string::npos){
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        result.push_back(token);
    }
    result.push_back(s.substr(pos_start));
    return result;
}

std::string CLevel::PutJson(){
    std::string key = "";
    std::string value = "";

    std::ifstream f_json(FLAGS_json);
    Json::Reader reader;
    Json::Value root;

    if(f_json.is_open() && reader.parse(f_json, root)){
        std::string id = root["id"].asString();
        std::string type = root["type"].asString();
        std::string timestamp = root["timestamp"].asString();
        value = root["content"].asString();

        key.append(id);
        key.append("_");
        key.append(type);
        key.append("_");
        key.append(timestamp);   

        //std::cout << "Key: " << key << "   Value: " << value << std::endl;     

        leveldb::DB* db;
        leveldb::WriteOptions wopt;
        leveldb::ReadOptions ropt;
        leveldb::WriteBatch batch;
        LevelDB ldb(db, wopt, ropt, batch);
        ldb.OpenDatabase(FLAGS_db.c_str());

        ldb.Put(key, value); 

        ldb.CloseDatabase();

    }

    return key;
}

unsigned long CLevel::PutDir(){
    //std::cout << "point 3: FLAGS_dir: " << FLAGS_dir << std::endl;
    //system("rm -rf /home/nemo/Desktop/demoday/db");

    std::ofstream f_log;
    f_log.open("log.txt", std::ios_base::app);


    leveldb::DB* db;
    leveldb::WriteOptions wopt;
    leveldb::ReadOptions ropt;
    leveldb::WriteBatch batch;
    LevelDB ldb(db, wopt, ropt, batch);
    ldb.OpenDatabase(FLAGS_db.c_str());

    DIR *dr;
    struct dirent *de;
    std::string f_url;

    std::ifstream f_in;
    int key = 1, size;
    unsigned long total_size = 0;
 
    //Metadata metadata;

    dr = opendir(FLAGS_dir.c_str());
    if(dr == NULL){
        std::cout << "Could not open current directory" << std::endl;
	    return 0;
    }

    //read all files from directory
    while((de = readdir(dr)) != NULL){
	// - - - - - Preprocessing part starts - - - - - 
        if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0){
	        continue;
	    }
        f_url = "";
        f_url.append(FLAGS_dir);
        f_url.append("/");
        f_url.append(de->d_name);
        f_in.open(f_url);
        if(!f_in.is_open()){
            std::cout << "Unable to open the file!" << std::endl;
            f_in.close();	
            continue;
        }
        std::string v((std::istreambuf_iterator<char>(f_in)), (std::istreambuf_iterator<char>()));
        size = v.length();
        total_size += size;
        // - - - - - Preprocessing part ends - - - - - 

        // - - - - - leveldb part starts - - - - - - -
        std::string generated_key = generateKey(key);
        ldb.Put(generated_key, v);    
        f_log << "File: " << f_url << ", " << "Key: " << generated_key << std::endl;
        key++;
        f_in.close();
    }
    f_log.close();

    ldb.CloseDatabase();

    return total_size;

}

std::string CLevel::Get(std::string key){
    leveldb::DB* db;
    leveldb::WriteOptions wopt;
    leveldb::ReadOptions ropt;
    leveldb::WriteBatch batch;
    LevelDB ldb(db, wopt, ropt, batch);
    ldb.OpenDatabase(FLAGS_db.c_str());

    std::string value = ldb.Get(key);

    //std::ofstream f_out ("read/file_1.jpg", std::ofstream::binary);
    //unsigned int size = value.size();
    //std::cout<<"Size: " << size << std::endl;

    if(!value.empty()){
	//f_out.write(reinterpret_cast<char *>(&size), sizeof(size) );
	//f_out.write(value.c_str(), value.size());
        std::vector<std::string> keys = split(key, "_");
        std::ofstream f_out("data_out.json");
        Json::Value root;
        root["id"] = keys.at(0);
        root["type"] = keys.at(1);
        root["timestamp"] = keys.at(2);
        root["content"] = value;

        Json::StyledWriter styled_writer;
        f_out << styled_writer.write(root);
        f_out.close();
    }	
   // f_out.close();

    return ldb.Get(key);
}

void CLevel::Getall(){
    std::cout<<"Data will be retreived from " << FLAGS_db << std::endl;
    //print out the database path.

    std::ofstream f_log;
    f_log.open("log.txt", std::ios_base::app);
    //create a log file to make sure.

    leveldb::DB* db;
    leveldb::WriteOptions wopt;
    leveldb::ReadOptions ropt;
    leveldb::WriteBatch batch;
    LevelDB ldb(db, wopt, ropt, batch);
    ldb.OpenDatabase(FLAGS_db.c_str());
    //open a leveldb.

    DIR *dr;
    struct dirent *de;
    //open *dr and *de for file output.

    std::string f_file;
    std::string f_filename;
    std::ifstream f_o;
    std::ofstream f_out;


    dr = opendir(FLAGS_db.c_str());
    std::cout << "Starting iteration..." << std::endl;
    if(dr==NULL){
        std::cout << "Could not open the directory folder" << std::endl;
    }//if db path is uncorrect, print an error message

    leveldb::Iterator* it = db->NewIterator(ropt);

    std::cout << "Start Iteration" << std::endl;
    
    for(it->SeekToFirst(); it->Valid();it->Next()){
        std::cout <<it->key().ToString() << std::endl;
        //(it->value().ToString()) >> folder

        f_filename = it->key().ToString();
        f_file = "";
        f_file.append(it->value().ToString());
        f_o.open(f_file);
        std::string ov((std::istreambuf_iterator<char>(f_o) ), (std::istreambuf_iterator<char>()) );
        f_out.open(f_filename + ".jpg", std::ios_base::app);
        f_out << ov;
        f_log << "File " << it->key().ToString() << " is retrieved" << std::endl;
        f_o.close();
        f_out.close();
    }
    
    f_log.close();
}
void CLevel::Delete(std::string key){}
