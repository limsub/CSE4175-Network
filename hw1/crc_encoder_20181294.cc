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

// 모든 문자가 ascii code 내에 있다는 걸 가정하고 풀었다. (0 - 127)

int main(int argc, char *argv[]) {

    

    // 1. 인자의 수가 맞지 않은 경우
    if (argc != 5) {
        printf("usage: ./crc_encoder input_file output_file generator dataword_size");
        exit(0);
    }

    // 2. input_file을 open하지 못한 경우
    ifstream readFile;
    readFile.open(argv[1]);

    string input_file = "";
    string line;

    if (readFile.is_open() ) { // 성공적으로 열렸을 때
        while(getline(readFile, line)) {
            input_file += line;
            input_file += "\n"; // string input_file에 문자열을 넣어준다. 줄넘김까지 표시해준다.
        }
        input_file = input_file.substr(0, input_file.length() - 1);
        readFile.close();
    }
    else {
        cerr << "input file open error." << endl;
        exit(0);
    }
    //cout << input_file << endl;


    // 3. output_file을 open하지 못한 경우
    ofstream outfile(argv[2], ios::out | ios::binary);
    if (!outfile) {
        cerr << "output file open error." << endl;
        exit(0);
    }

    // 4. dataword_size가 4 또는 8이 아닌 경우
    string a4 = argv[4];
    if ( a4 != "4" && a4 != "8" ) {
        cerr << "dataword size must be 4 or 8." << endl;
        exit(0);
    }


    // generator int로 바꿔주기
    // generator가 0부터 시작하는 경우를 생각해 주어야 한다!
    // msbposition(generator)를 하나의 변수로 잡아주고, generator는 어차피 비트로 바뀐 후 정수로 변환되기 때문에 상관 없다. 나중에 다시 비트로 변환한다.
    int generator_size = strlen(argv[3]);
    
    string initial_generator = argv[3];

    int generator = (bitset<32>(initial_generator)).to_ulong();
    int dataword_size = atoi(argv[4]);

    // input_file을 dataword_size로 나눠서, 4 or 8짜리 dataword들의 리스트로 만들자
    int len = input_file.length();

    vector<string> bits_in_str; 


    // 나누기를 위해 뒤에 이어 붙일 비트
    string plus_for_divide = "";
    for (int i = 0; i < msbposition(generator) - 1; i++) {
        plus_for_divide += '0';
    }

    for (int i = 0; i < len; i++) {
        //printf("%c : ", input_file[i]);
        //cout << bitset<8> (input_file[i]) << endl;

        bitset<8> bit(input_file[i]);
        string bit_str = bit.to_string();

        for (int j = 0; j < 8; j+= dataword_size) {
            bits_in_str.push_back(bit_str.substr(j, dataword_size) + plus_for_divide);

            //cout << bit_str.substr(j, dataword_size) << " "; // j 부터 4칸 더 출력
        }

        //printf("\n");
    }
/*
    printf("\n *현재 리스트에 들어있는 문자열들을 출력합니다. 모두 bit가 아니라 문자열 상태입니다. : \n");
    for (auto it = bits_in_str.cbegin(); it != bits_in_str.cend(); it++) {
        cout << *it << endl;
    }
*/

    // divisor 정리
    //cout << "현재 divisor : " << bitset<8>(generator) << endl;
    string all_generator = (bitset<8>(generator)).to_string();
    string real_generator;
    for (int i = 0; i <= all_generator.size(); i++) {
        if (all_generator[i] == '1') {
            real_generator = all_generator.substr(i, all_generator.size() - i);
            break;
        }
    }
    //cout << "앞에 0을 제거한 divisor 문자열 : " << real_generator << endl;
    

    // 리스트에 있는 각 문자들 (bit들)에 대해 generator로 나누고, 뒤에 붙어있던 000 대신 나머지를 붙여준다.
    // cspro에서 실행시켰을 때, bit연산 right/left shift할 때 모두 빈공간 0 붙인다.

    // 지금 원소들의 크기를 생각하면, dataword(4 or 8) + (generator size - 1). generator size maximum이 8이라고 치면, 아무리 커도 16 size 안에 다 쓸 수 있다.
    // 그렇다면, 일단 bitset<16>으로 변환해주고, 앞에 써있을 0들을 shift로 다 날려주자.
    // data << (16 - (dataword + msbposition(generator) - 1))
    // size를 맞춰주기 위해, generator도 size 16으로 해주고, << (16 - msbposition(generator))

    // 나머지를 더해서 계산이 끝난 codeword를 넣어줄 list
    vector<string> final_codeword;

    //auto it = bits_in_str.cbegin(); it != bits_in_str.cend(); it++
    for (int i = 0; i < bits_in_str.size(); i++) {
        string a = bits_in_str[i];

        bitset<16> data = bitset<16>(a);
        
        bitset<16> generator_for_divide = bitset<16>(generator);
        string remainder = "";
        

        generator_for_divide = generator_for_divide << (16 - msbposition(generator));
        data = data << (16 - (dataword_size + msbposition(generator) - 1));
        string strdata = data.to_string();

        //cout << "data : " << data << endl;
        //cout << "generator : " << generator_for_divide << endl;

        // 사실상 중요한 건 맨 왼쪽에 dataword_size만큼의 bit이고, 뒤에 붙어있는 0들은 필요 없다. 여유분으로 남겨둔 것.
        for (int i = 0; i < dataword_size; i++) {

            //cout << "i : " << i << endl;
            //cout << "strdata : " << strdata << endl;

            if (strdata[i] == '1') {
                //cout << "strdata[i] is 1" << endl;

                data = data ^ generator_for_divide;
                strdata = data.to_string();

                //cout << "data becomes : " << data << endl;
                //cout << "strdata : " << strdata << endl;
            }

            generator_for_divide = generator_for_divide >> 1;

            //cout << "generator becomes : " << generator_for_divide << endl;

            //printf("\n\n");
        }
        remainder = data.to_string();

        // remainder의 size를 맞춰준다 (generator size - 1)
        // 이 때, remainder가 모두 0일 때의 case도 고려한다.

        //cout << "original remainder : " << remainder << endl;
        remainder = remainder.substr(dataword_size, msbposition(generator) - 1);
        /*if (remainder.find('1') == string::npos) {
            remainder = "";
            for (int i = 0; i < msbposition(generator) - 1; i++) {
                remainder += '0';
            }
        } 
        else {
            for (int p = 0; p < remainder.size(); p++) {
                if ( remainder[p] == '1') {
                    remainder = remainder.substr(p, msbposition(generator) - 1);
                    break;
                }
            }
        }*/
        //cout << "remainder : " << remainder <<endl;

        a = a.substr(0, dataword_size) + remainder;
        //cout << "final codeword : " << a << endl;

        final_codeword.push_back(a);
    }

/*
    printf("\n *현재 리스트에 들어있는 문자열들을 출력합니다. 모두 bit가 아니라 문자열 상태입니다. : \n");
    for (auto it = bits_in_str.cbegin(); it != bits_in_str.cend(); it++) {
        cout << *it << endl;
    }

    printf("\n *현재 리스트에 들어있는 문자열들을 출력합니다. 모두 bit가 아니라 문자열 상태입니다. : \n");
    for (auto it = final_codeword.cbegin(); it != final_codeword.cend(); it++) {
        cout << *it << endl;
    }

    */

    // zero padding. 16비트로 나눠지지 않으면 맨 앞에 0으로 패딩을 주어야 한다.
    int padding_size = 0;
    int sum = (dataword_size + msbposition(generator) - 1) * final_codeword.size();
    padding_size = 16 - (sum%16);

    //cout << "padding size : " << padding_size << endl;

    // 맨 앞에 주는 padding
    string zero_padding;
    for (int i = 0; i < padding_size; i++) {
        zero_padding += '0';
    }


    // output file
    // 1. padding_size
    bitset<8> bit_padding_size(padding_size);
    // 2. zero padding
    bitset<8> bit_zero_padding(zero_padding);
    // 3. final codeword


    string final_codeword_string = "";

    for (int i = 0; i < final_codeword.size(); i++) {
        string a = final_codeword[i];
    
        //cout << a << endl;
        final_codeword_string += a;
    }

    string final_output = bit_padding_size.to_string() + zero_padding + final_codeword_string;

    //cout << final_output << endl;

    // bit string 을 바이트로 변환하여 파일에 쓰기
    for (size_t i = 0; i < final_output.length(); i += 8) {
        string byte_str = final_output.substr(i, 8);
        bitset<8> byte(byte_str);
        unsigned char c = static_cast<unsigned char>(byte.to_ulong());
        outfile.write(reinterpret_cast<const char*>(&c), sizeof(c));
    }
    outfile.close();
    return 0;



}