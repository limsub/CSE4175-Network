#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <cmath>
#include <list>
#include <fstream>
#include <stdlib.h>
#include <cstring>


using namespace std;

int msbposition(int x) {

    int bit_position = static_cast<int>(ceil(log2(x)));
    return bit_position;
}

int main(int argc, char*argv[]) {

    // 1. 인자의 수가 맞지 않은 경우
    if (argc != 6) {
        printf("usage: ./crc_decoder input_file output_file result_file generator dataword_size");
        exit(0);
    }

    // 2. input_file을 open하지 못한 경우
    string input_string;
    char byte;

    ifstream input_file;
    input_file.open(argv[1], ios::binary);
    string line;
    if (input_file.is_open() ) {
        while (input_file.get(byte)) {
            bitset<8> bits(byte);
            input_string += bits.to_string();
        }
        input_file.close();

        /*if (input_string.length() % 8 != 0) {
        input_string += string(8 - input_string.length() % 8, '0');
        
        }*/
    }
    else {
        cerr << "input file open error." << endl;
        exit(0);
    }
    

    // 3. output_file을 open하지 못한 경우
    ofstream outfile;
    outfile.open(argv[2]);
    if (!outfile) {
        cerr << "output file open error." << endl;
        exit(0);
    }

    // 3.5. result_file을 open하지 못한 경우
    ofstream resultfile;
    resultfile.open(argv[3]);
    if (!resultfile) {
        cerr << "result file open error." << endl;
        exit(0);
    }


    // 4. dataword_size가 4 또는 8이 아닌 경우
    string a4 = argv[5];
    if ( a4 != "4" && a4 != "8" ) {
        cerr << "dataword size must be 4 or 8." << endl;
        exit(0);
    }

    int generator_size = strlen(argv[4]);
    string initial_generator = argv[4];

    int generator = (bitset<32>(initial_generator)).to_ulong();
    int dataword_size = atoi(argv[5]);


    int total_num = 0;
    int fail_num = 0;

    // 1. zero padding이 몇인지 먼저 8칸 읽어
    bitset<8> bit_zero_padding_size(input_string.substr(0, 8));
    int zero_padding_size = (bitset<8> (input_string.substr(0, 8)) ).to_ulong();

    int k =dataword_size + msbposition(generator) - 1; // 7 or 11


    // 2. 처음 8칸 + zero_padding_size 만큼 제외하고 그 다음부터 7 or 11개(dataword size + msb(generator)씩 리스트에 저장.
    // 리스트의 길이 / (7 or 11) 만큼 loop 을 돌자

    vector<string> regenerateword;
    total_num = (input_string.length() - 8 - zero_padding_size)/7;
    //cout << "total num : " << total_num << endl;
    //cout << dataword_size + msbposition(generator) - 1 << endl;
    for (int i = 0; i < (input_string.length() - 8 - zero_padding_size) / (dataword_size + msbposition(generator) - 1); i++) {
        
        // dataword만큼의 진짜 비트들을 복원 목적으로 받아온다.
        regenerateword.push_back(input_string.substr(8 + zero_padding_size + i*k, dataword_size));

        string x = input_string.substr(8 + zero_padding_size + i*k, k );
        
        bitset<16> bit_str(x);
        bitset<16> generator_for_divide(generator);

        bit_str = bit_str << 16 - k;
        generator_for_divide = generator_for_divide << 16 - msbposition(generator);
        string strdata = bit_str.to_string();

        for (int i = 0; i < dataword_size; i++) {
            if (strdata[i] == '1') {
                bit_str = bit_str ^ generator_for_divide;
                strdata = bit_str.to_string();
            }
            generator_for_divide = generator_for_divide >> 1;
        }

        //cout << "remainder : " << bit_str.to_ullong() << endl;

        if (bit_str.to_ulong() != 0) {
            //cout << "fail in " << i << endl;
            fail_num++;
        }



        //cout << x << endl;
        //cout << bit_str << endl;
        //cout << generator_for_divide << endl;
    }

    //cout << "fail num : " << fail_num << endl;

    string regeneratestring = "";

    for (int i = 0; i < regenerateword.size(); i++) {
        string a = regenerateword[i];
        regeneratestring += a;
    }

    //cout << regeneratestring << endl;

    string final_str = "";
    for (int i = 0; i < regeneratestring.length(); i+= 8) {
        bitset<8> y(regeneratestring.substr(i, 8));
        char z = char(y.to_ulong());
        final_str += char( (bitset<8>(regeneratestring.substr(i, 8)).to_ulong() ) );
        //cout << z << endl;
    }



    outfile << final_str << endl;

    resultfile << total_num << " " << fail_num << endl;
    

    return 0;
}