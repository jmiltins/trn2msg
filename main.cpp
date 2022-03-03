#include <iostream>
#include <fstream>
#include <time.h>
/*Transactions are presented in fixed length format file separated by newline character:
Data field Siz
e
Name

1 2 Transaction type (00 � purchase, 01 � withdrawal)
2 16 Primary account number (PAN)
3 12 Amount in submits
4 14 Transaction time (yyyymmddhh24miss)
5 3 Currency code in numeric format
Possible currency values (all have exponent 2 � number of subunits in unit is 100):
Numeric code Alpha code
840 usd
978 eur
826 gbp
643 rub
Notification file

&lt;msg&gt;Purchase with card 966796******6093 on 11.11.2018 14:34,
amount 45.99 usd.&lt;/msg&gt;
&lt;msg&gt;Withdrawal with card 8237******105879 on 11.12.2018 09:56,
amount 10.00 eur.&lt;/msg&gt;

*/

using namespace std;
uint16_t cnt = 0; // Max 65536
float totalSum = 0; // max 3.40282e+038


// Transastion type check f-tion
string checkTransactionType(string id, int pos = 0, int lenght = 2)
{
    if (id.substr (pos,lenght) == "00")
    {
        return "Purchase";
    }
    else if(id.substr (pos,lenght) == "01")
    {
        return "Withdrawal";
    }
    else
    {
        return "None";
    }
}

// Account(pan) return f-tion + masked pan 6+4
string returnAccount(string id, int pos = 2, int lenght = 10)
{
    //cout << "account" <<": "  << id.substr (pos,lenght)<< endl;
    string account = id.substr (pos,lenght);
    //replace (size_t pos,  size_t len,  size_t n, char c);https://www.cplusplus.com/reference/string/string/replace/
    if (id.substr (0, 2) == "00")  // for purchase
    {
        account.replace(6,0,6,'*');
    }
    else
    {
        account.replace(4,0,6,'*');
    }
    return account;
}
// Amount return f-tion
string returnAmount(string id, int pos = 18, int lenght = 12) // 12
{
    //cout << "account" <<": "  << id.substr (pos,lenght)<< endl;
    double amountDouble = stof((id.substr (pos,10))+ "." + (id.substr (28,2)));

    string amountString =  (to_string(amountDouble));
    // remove digit after to_string() ftion, because it returns with .000000, need just .00 .
    for (int i = 0; i < 4; i++)
    {
        amountString.pop_back(); // remove digit after .
    }
    //float amountFraction = (stof(id.substr (28,2)))/100;

    return amountString;//amountWhole + amountFraction ;//id.substr (pos,lenght);
}
// Time extract convert and return f-tion, 14 Transaction time (yyyymmddhh24miss) =>  11.11.2018 14:34
//11.11.2018 14:34,
string returnTime(string id)
{
    // (yyyymmddhh24miss) =>  11.11.2018 14:34
    string time = id.substr (34,2)+ "." + id.substr (36,2)+ "." + id.substr (30,4) +  " " + id.substr (38,2)+ ":" + id.substr (40,2);
    //cout << "time" <<": "  << time<< endl;
    return time;
}

// Currency check f-tion
string returnCurr(string id, int pos = 44, int lenght = 3) //44, 3
{
    // f-tion checks digits in position, returns string currency
    switch (stoi(id.substr (pos,lenght)))
    {
    case 840:
        return "usd";
        break;
    case 978:
        return "eur";
        break;
    case 826:
        return "gbp";
        break;
    case 643:
        return "rub";
        break;
    default:
        return "None";
        break;
    }
}
// checks id for invalid or missing data, if no data, returns false
bool checkForErrors(string id)
{
    bool validation = true;
    if (id.length() != 47)
    {
        validation = false;
    }
    else if (returnCurr(id) == "None")
    {
        validation = false;
    }
    return validation;
}
// Result Output format f-tion + total amount and transaction count add
string outputMessageConsrtuct(string id)
{
    string outputMessage = "";


    if (checkForErrors(id)) // if there is/are errors, does not adds to total sum
    {
        //cout<< checkTransactionType(id)<<"  with card  "<<returnAccount(id)<<" on "<<returnTime(id) << ", amount "<< returnAmount(id)<< returnCurr(id)<<"."<<endl;
        outputMessage = (checkTransactionType(id)) + "  with card  " + (returnAccount(id)) + " on " + (returnTime(id)) + ", amount " + (returnAmount(id)) + (returnCurr(id)) + ".";
        // add transaction count
        cnt ++;
        // add total amount summ
        totalSum += stof(returnAmount(id));
        //cout <<" sum "<<totalSum<<endl;
    }
    return outputMessage;
}

int main()
{
    // "00966796969690609300000000459920181111143445840"9667******969696
    // "01234567890123456789012345678901234567890123456"
    // "           1        2          3        4
// Print "Output:"
    cout <<"Output: "<< endl;
// read from text file transactions and create log file

    ifstream fin; //read from file
    ofstream fin2; //create and write log file
    string logFileName = "test2.txt";

    fin.open("transactions2.txt"); //open txt
    fin2.open(logFileName);
    string newLine;
    if(fin.is_open() && fin2.is_open()) // check is both files are open
    {
        //create start message for the log file "17:30:05.001 Start of file conversion"
        fin2 << logFileName <<"Start of file conversion\n\n"; // log file start

        //17:30:05.001 ERROR: Invalid arguments given
        //

        while (getline(fin,newLine))
        {
            fin2 << newLine <<"\n";
            cout << outputMessageConsrtuct(newLine) << endl;

        }
    }
    //create end message for the log file  "17:30:06.020 End of conversion"
    fin2 << logFileName <<"End of conversion"; // log file end

    fin.close(); // close file
    fin2.close();// close file2

// Print totals
//totals cnt="2" sum="5599" date="2018.11.17 12:34:45"/
    cout<<"Total transactions = "<<cnt<<" total amount: "<< totalSum << endl;
// declaring argument of time()
    time_t my_time = time(NULL);


    newLine = printf ("\ntotals cnt=\"%u\" sum=\"%8.2f\" date=\"%s\"/", cnt, totalSum, ctime(&my_time));

    cout<<"\nnewLine"<<newLine<<endl;
// Time
//cout<<;//ctime();


    // ctime() used to give the present time
    printf("%s", ctime(&my_time));

    return 0;
}