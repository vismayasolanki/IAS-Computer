// BY vismaya solanki IMT2019095
   //first there will be a jump to the pc = 0
    //Let's say there are two operations to perform one is c = a + b;
    /*
    following is the assembly for the operation required to 
    Memory : a @ 500 
             b @ 501
             c @ 502
    Load(a), Add(b) --- (1)
    Lsh(),stor(c)


    */



#include<bits/stdc++.h>
using namespace std;
bool memory[1000][40];//as the word is of 40 bit
string pc = "000000000011";
bool pc_binary[12];
bool ACC[40];
bool MQ[40];
bool Q[40];
bool IR[8];
bool MAR[12];
bool MBR[40];
bool IBR[20];
bool to_halt = false;
bool in_IBR = false;
bool jump_left = false;
bool jump_right = false;
bool to_jump = false;

int binary_to_decimal(string binary,bool is_data){
    int decimal = 0;
    int sign = ((char)binary[0] == '0') ? 1 : -1;
    reverse(binary.begin(),binary.end());
    int f = 1;
    if(is_data){
        for(int i=0;i<binary.length()-1;i++){
        // for(char c : binary){
            // if(c == '1'){
            if((char)binary[i] == '1'){
                decimal += f;
            }
            f *= 2;
        }
        // return (binary[0] - '0' == 1) ? -1*decimal : decimal;
        return sign * decimal;
        // return decimal;
    }
    else{
        for(char c : binary){
            if(c == '1'){
                decimal += f;
            }
            f *= 2;
        }
        return decimal;
    }
}

string decimal_to_binary(int n,int no_expected_bits){
    string binary = "";
    char sign = (n >= 0) ? '0' : '1';
    n = abs(n);
    while(n){
        binary += to_string(n%2);
        n /= 2;

    }
    int tmp = binary.length();
    for(int i=0;i<no_expected_bits-tmp;i++){
        binary += "0";
    }
    reverse(binary.begin(),binary.end());
    binary[0] = (char)sign;
    return binary;
}

bool add_bits(bool a, bool b,bool *carry){
    if(a && b && *carry){ 
        *carry = 1;
        return 1;
    }

    else if((a && b) || (a && *carry) || (b && *carry)){
        *carry = 1;
        return 0;
    }
    else if(a || b || *carry){
        *carry = 0;
        return 1;
    }
    else{
        return 0;
    }
}

string binary_addition(string a, string b,int no_expected_bits){
    bool is_data = true;
    if(no_expected_bits == 40)is_data = true;
    else is_data = false;
    int decimal_a = binary_to_decimal(a,is_data);
    int decimal_b = binary_to_decimal(b,is_data);
    return decimal_to_binary(decimal_a + decimal_b,no_expected_bits);
}

void fetch(string address){
    // it will just bring all the elements in MBR
    int decimal_add = binary_to_decimal(address,false);
    // cout<<"pc for fetch is "<<decimal_add<<endl;
    for(int i=0;i<40;i++){
        MBR[i] = memory[decimal_add][i];
    }
    int i;
    for(i=0;i<8;i++){
        if(MBR[i]){
            break;
        }
    }
    if(i < 8){
        // both instructions are there
        for(int i=0;i<20;i++){
            IBR[i] = MBR[i+20];
        }
        for(int i=0;i<8;i++){
            IR[i] = MBR[i];
        }
        for(int i=0;i<12;i++){
            MAR[i] = MBR[i+8];
        }
        in_IBR = true;
    }
    else{
        in_IBR = false;
        for(int i=0;i<20;i++){
            
        }
        for(int i=0;i<8;i++){
            IR[i] = MBR[20+i];
        }
        for(int i=0;i<12;i++){
            MAR[i] = MBR[i+8];
        }
    }
}

void fetch_data(string address){
    // string add = "";
    // for(int i=0;i<12;i++){
    //     add += to_string(address[i]);
    // }
    int decimal_address = binary_to_decimal(address,false);
    // put it down in MBR
    for(int i=0;i<40;i++){
        MBR[i] = memory[decimal_address][i];
    }
}

// instructions 

void LSH(){
    // This will multiply ACC by 2
    // sign bit will remain intact
    cout<<"Left shifting"<<endl;
    for(int i=1;i<39;i++){
        ACC[i] = ACC[i+1];
    }
    ACC[39] = 0;


}

void RSH(){
    // This will divide ACC by 2
    // Sign bit will remain intact
    cout<<"Right shifting"<<endl;
    for(int i=39;i>1;i--){
        ACC[i] = ACC[i-1];
    }
    ACC[0] = 0;
}

void Sub_mod(string address){
    cout<<"Substracting Mod(x) "<<endl;
    fetch_data(address);
    string acc = "",mbr = "";
    for(int i=0;i<40;i++){
        acc += to_string(ACC[i]);
        mbr += to_string(MBR[i]);
    }
    mbr[0] = '1';
    string tmp = binary_addition(acc,mbr,40);
    for(int i=0;i<40;i++){
        ACC[i] = tmp[i] - '0';
    }
}

void Load(string address){
    // here we have to load contents of M[add] into accumulator
    cout<<"loading"<<endl;
    int decimal_address = binary_to_decimal(address,false);
    for(int i=0;i<40;i++){
        ACC[i] = memory[decimal_address][i];
    }
}

void Add(string address){
    //here we have to fetch the contents of M[add] into MBR and then AC <- Ac + MBR
    cout<<"adding"<<endl;
    fetch_data(address);
    string acc = "",mbr = "";
    for(int i=0;i<40;i++){
        acc += to_string(ACC[i]);
        mbr += to_string(MBR[i]);
    }
    string new_acc = binary_addition(acc,mbr,40);
    for(int i=0;i<40;i++){
        ACC[i] = new_acc[i] - '0';
    }
    //after this the new accumulator will have added contents
}

void Add_mod(string address){
    cout<<"Adding Mod(x)"<<endl;
    fetch_data(address);
    string acc = "",mbr = "";
    for(int i=0;i<40;i++){
        acc += to_string(ACC[i]);
        mbr += to_string(MBR[i]);
    }
    mbr[0] = '0';//Taking | M[x] |
    string new_acc = binary_addition(acc,mbr,40);
    for(int i=0;i<40;i++){
        ACC[i] = new_acc[i] - '0';
    }
}

void StorMx(string address){
    // here the contents of ACC will be transferred to the provied location in memmory
    cout<<"storing"<<endl;
    int decimal_address = binary_to_decimal(address,false);
    for(int i=0;i<40;i++){
        memory[decimal_address][i] = ACC[i];
    }
}

void Sub(string address){
    cout<<"Substracting"<<endl;
    fetch_data(address);
    string acc = "",mbr = "";
    for(int i=0;i<40;i++){
        acc += to_string(ACC[i]);
        mbr += to_string(MBR[i]);
    }
    mbr[0] = (mbr[0] == '1') ? '0' : '1';
    // cout<<"sub here"<<binary_to_decimal(acc,true)<<"   "<<binary_to_decimal(mbr,true)<<endl;
    string tmp = binary_addition(acc,mbr,40);
    // cout<<"sub is "<<binary_to_decimal(tmp,true)<<endl;
    for(int i=0;i<40;i++){
        ACC[i] = tmp[i] - '0';
    }
}

void conditional_jump_left(string address){
    //Here we have to jump from the current location to the given location 
    // we will increment the pc for that purpose 
    cout<<"Conditional Jumping left"<<endl;
    string acc = "";
    for(int i=0;i<40;i++){
        acc += to_string(ACC[i]);
    }
    int decimal_acc = binary_to_decimal(acc,true);
    if(decimal_acc >= 0){
        pc = address;
        fetch(pc);
        to_jump = true;
        jump_left = true;
    }
    else to_jump = false;
}

void conditional_jump_right(string address){
    cout<<"Conditional Jumping Right"<<endl;
    string acc = "";
    for(int i=0;i<40;i++){
        acc += to_string(ACC[i]);
    }
    int decimal_acc = binary_to_decimal(acc,true);
    if(decimal_acc >= 0){
        pc = address;
        fetch(pc);
        jump_right = true;
        to_jump = true;
    }
    else to_jump = false;
}

void unconditional_jump_left(string address){
    cout<<"Unconditional Jumping left"<<endl;
    pc = address;
    fetch(pc);
    to_jump = true;
    jump_left = true;

}

void uncoditional_jump_right(string address){
    cout<<"Unconditional Jumping right"<<endl;
    pc = address;
    fetch(pc);
    to_jump = true;
    jump_right = true;
}

void Load_MQ(){
    cout<<"Loading MQ"<<endl;
    for(int i=0;i<40;i++){
        ACC[i] = MQ[i];
    }
}

void Load_MQ_MX(string address){
    cout<<"Loading MQ MX"<<endl;
    int decimal_address = binary_to_decimal(address,false);
    for(int i=0;i<40;i++){
        MQ[i] = memory[decimal_address][i];
    }
}

void Load_neg_MX(string address){
    cout<<"Loading Negative of M(X)"<<endl;
    int decimal_address = binary_to_decimal(address,false);
    for(int i=0;i<40;i++){
        ACC[i] = memory[decimal_address][i];
    }
    // making it negative
    ACC[0] = (ACC[0] == 1) ? 0 : 1;
}

void execute(bool is_right){
//this will decode and execute use switch case statements as decode
// now as we are executing we assume that we completed the fetch operation and the MBR has the correct instruction
    string op,address;
    for(int i=0;i<8;i++)op += to_string(IR[i]);
    for(int i=0;i<12;i++)address += to_string(MAR[i]);
    // op = instruction.substr(0,8); 
    // address = instruction.substr(8,12);
    int decimal_op = binary_to_decimal(op,false);
    if(is_right){//if the right instruction's opcode is zero then its halt
        if(decimal_op == 0){
            to_halt = true;
            return;
        }
    }
    // cout<<"opcode : " <<decimal_op<<endl;
    switch (decimal_op){
        case 1:
            Load(address);
            break;
        case 33:
            StorMx(address);
            break;
        case 5:
            Add(address);
            break;
        case 6:
            Sub(address);
            break;
        case 15:
            conditional_jump_left(address);
            break;
        case 16:
            conditional_jump_right(address);
            break; 
        case 0:
            break;
        case 20:
            LSH();
            break;
        case 21:
            RSH();
            break;
        case 8:
            Sub_mod(address);
            break;
        case 7:
            Add_mod(address);
            break;
        case 14:
            uncoditional_jump_right(address);
            break;
        case 13:
            unconditional_jump_left(address);
            break;
        case 10:
            Load_MQ();
            break;
        case 9:
            Load_MQ_MX(address);
            break;
        case 2:
            Load_neg_MX(address);
            break;
        default:
            // ERROR -- no such instruction implemented
            break;
    }

}

void execute_util(){//this is a 40-bit instruction
    if(!jump_right && in_IBR){
        execute(false);//left-instruction 
        in_IBR = false;
        for(int i=0;i<8;i++)IR[i] = IBR[i];
        for(int i=0;i<12;i++)MAR[i] = IBR[i+8];

    }
    if(!jump_left){
        execute(true);//right-instruction
    }
    jump_left = false;
    jump_right = false;
}



void cycle(){
    fetch(pc);//at beginning of every new cycle address will come from pc
    string mbr = "";
    for(bool x : MBR){
        mbr += to_string(x);
    }
    execute_util();
    // execute(mbr.substr(0,20),false);//left-instruction 
    // execute(mbr.substr(20,20),true);//right-instruction
    
    if(!to_jump){
        pc = binary_addition(pc,"1",12);
    }
    to_jump = false;

}

void hardcode_memory(){
    memset(memory,0,sizeof(memory));
    //first there will be a jump to the pc = 0
    //Let's say there are two operations to perform one is c = a + b;
    /*
    following is the assembly for the operation required to 
    Memory : a @ 500 
             b @ 501
             c @ 502
    Load(a), Add(b) --- (1)
    Lsh(),stor(c)


    */
   //Hardcoding the insttruction for first two memory words
    string first_word = "0000000100011111010000000101000111110101";
    for(int i=0;i<40;i++){
        memory[0][i] = first_word[i] - '0';
    }
    string second_word = "0001010000000000000000100001000111110110";
    for(int i=0;i<40;i++){
        memory[1][i] = second_word[i] - '0';
    }
    // cout<<"memory test";
    // cout<<memory[0][1]<<memory[1][4]<<endl;
    //harcoding unconditional jump
    string third_word = "0000000000000000000000001101000000000000";
    for(int i=0;i<40;i++){
        memory[3][i] = third_word[i] - '0';
    }    

    //Hardcoding the data
    //putting a = 10 in 500
    string a = "0000000000000000000000000000000000001010"; 
    for(int i=0;i<40;i++){
        memory[500][i] = a[i] - '0';
    }
    //putting b = 5 in 501
    string b = "0000000000000000000000000000000000000101";
    for(int i=0;i<40;i++){
        memory[501][i] = b[i] - '0';
    }


}



int main(){
    hardcode_memory();//successful
    // This is the code for c = a + b 
    // Here a = 10 and b = 5
    // pc here is initialized at 3 and we unconditionally jump from pc = 3 to pc = 0
    //Then we load(a) Add(b)
    // then stor(c)
    // then all 000... means halt()
    // cout<<"initial pc "<<pc<<endl;
    while(!to_halt){
        cycle();
        cout<<endl;
// 
    }
    cout<<"final result of c at memory[502] is displayed below"<<endl;
    for(int i=0;i<40;i++){
        cout<<memory[502][i];
    }
    cout<<endl;   
    return 0;
}