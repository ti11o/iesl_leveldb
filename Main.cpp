// g++ -I include/ Main.cpp LevelDB.cpp CLevel.cpp -o db_run -lleveldb -ljsoncpp
// ./db_run bin_files

#include "CLevel.h"

// 1: Put
// 2: Get
// 3: Get one
// 4: Delete
std::string FLAGS_op;

std::string FLAGS_db;

std::string FLAGS_dir;

std::string FLAGS_key;

std::string FLAGS_json;

unsigned long FLAGS_threshold_1 = 1024;

unsigned long FLAGS_threshold_2 = 65536;

unsigned long FLAGS_block = 4096;

std::string FLAGS_delimiter = "_@_";

void testPutJson(){
	CLevel clevel;
    std::cout << "Key: " << clevel.PutJson() << std::endl;
}

void testPutDir(){
    //std::cout << "point 2: FLAGS_dir: " << FLAGS_dir << std::endl;
    std::ofstream f_out;
    clock_t c_start, c_end;
    double duration, speed;
    unsigned long totalSize = 0;

    c_start = clock();

    CLevel clevel;
    totalSize = clevel.PutDir();

    c_end = clock();
    duration = (c_end - c_start) / (double) CLOCKS_PER_SEC;
    speed = totalSize / duration / 1048576;
    f_out.open("result.txt", std::ios_base::app);
    f_out << totalSize << ", " << speed << std::endl;
    f_out.close();
}

void testGetAll(){
    CLevel clevel;
    clevel.Getall();
}

void testGetOne(){
    CLevel clevel;
    clevel.Get(FLAGS_key);
}

void testDelete(std::string key){

}

int main(int argc, const char * argv[])
{
   // system("rm -rf /home/nemo/Desktop/db/leveldb/testdb");
    for(int i = 1; i < argc; i++){
	int n;
	char junk;
	if(strncmp(argv[i], "--op=", 5) == 0){
		std::string temp(argv[i] + 5);
	    FLAGS_op = temp;
	} 
	else if(strncmp(argv[i], "--db=", 5) == 0){
	    std::string temp(argv[i] + 5);
	    FLAGS_db = temp;
	} 
	else if(strncmp(argv[i], "--dir=", 6) == 0){
	    std::string temp(argv[i] + 6);
	    FLAGS_dir = temp;
	} 
	else if(strncmp(argv[i], "--json=", 7) == 0){
	    std::string temp(argv[i] + 7);
	    FLAGS_json = temp;
	} 
	else if(strncmp(argv[i], "--key=", 6) == 0){
	    std::string temp(argv[i] + 6);
	    FLAGS_key = temp;
	} 
	else {
	    fprintf(stderr, "Invalid flag '%s'\n", argv[i]);
	    exit(1);
	}
    }

    if(FLAGS_db.empty()){
		std::string default_db_path = "/tmp/leveldb";
		FLAGS_db = default_db_path.c_str();
    }

	//std::cout<<"op: " << FLAGS_op << std::endl;
    if(FLAGS_op.compare("put_dir") == 0){
		//std::cout << "point 1: FLAGS_dir: " << FLAGS_dir << std::endl;
		if(FLAGS_dir.empty()){
			fprintf(stderr, "You need to select a data directory! Use --dir= flag\n");
			exit(1);
		}
		testPutDir();
    }else if(FLAGS_op.compare("put_json") == 0){
		if(FLAGS_json.empty()){
			fprintf(stderr, "You need to select a JSON file! Use --json= flag\n");
			exit(1);
		}
		testPutJson();
    }else if(FLAGS_op.compare("get") == 0){
		if(FLAGS_key.empty()){
			testGetAll();
		}else{
			testGetOne();
		}
    }else if(FLAGS_op.compare("delete") == 0){
		if(FLAGS_key.empty()){
			fprintf(stderr, "You need to select a key! Use --key= flag\n");
			exit(1);
		}
		testDelete(FLAGS_key);
    }else{
		fprintf(stderr, "Invalid operation! '%s'. Possible options: put_dir, put_json, get, delete\n", FLAGS_op.c_str());
	    exit(1);
	}

    return 0;
}
