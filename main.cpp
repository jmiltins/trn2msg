#include <iostream>
#include <fstream>
//#include <ctime> // for time
#include <iomanip> // for time
#include <sstream> // for time
#include <chrono>


/*Transactions are presented in fixed length format file separated by newline character:
Data field Siz
e
Name

1 2 Transaction type (00 – purchase, 01 – withdrawal)
2 16 Primary account number (PAN)
3 12 Amount in submits
4 14 Transaction time (yyyymmddhh24miss)
5 3 Currency code in numeric format
Possible currency values (all have exponent 2 – number of subunits in unit is 100):
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
double timer; // for milliseconds


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
    else if (checkTransactionType(id) == "None")
    {
        validation = false;
    }
    return validation;
}
// Result Output format f-tion (if valid data) + total amount and transaction count add, ERROR (if invalid data
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
    else
    {
        return "ERROR: Invalid arguments given";
    }
    return outputMessage;
}
// Calcilate millis since start + format .000
string returnMillisecFormat()
{
    float dividedResult = (clock() - timer) / 1000;
    string str = to_string(dividedResult);
    // remove digit after to_string() ftion, because it returns with .000000, need just .00 .
    for (int i = 0; i < 3; i++)
    {
        str.pop_back(); // remove digit after .
    }
    str.erase(0, 1); // remove digit front .
    return str;
}
// Split time string
// Create log time for Log file entries should begin with timestamp in the form HH: MM: ss.sss. f.ex. 17:30:05.001
string logTime()
{
    // Returns string in format 17:30:05.001
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S")<< returnMillisecFormat();
    auto str = oss.str();
    return str;
}
// create time stamp for xml file entries
string xmlFileTime()
{
    // Returns string in format YYYY.MM.DD HH24:MI:SS
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y.%m.%d %H:%M:%S"); // or "%Y-%m-%d %H:%M:%S"
    auto str = oss.str();
    return str;
}
// F-tion to create time stamp for log file name in format YYMMDD
string logFileNameTime()
{
    // Returns string in format YYMMDD
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%y%m%d");
    auto str = oss.str();
    return str;
}


int main()
{
// read from text file transactions and create log file and xml file
    ifstream fin; //read from .txt file
    ofstream fin2; //create and write log file
    ofstream xmlFile; //create and write xml file
    // create log file name, <program_name>_YYMMDD.log
    string logFileName = "trn2msg_" + (logFileNameTime()) + ".log";
    // create xml file name, <program_name>_YYMMDD.xml
    string xmlFileName = "trn2msg_" + (logFileNameTime()) + ".xml";

    fin.open("transactions2.txt"); //open txt
    fin2.open(logFileName); //create log
    xmlFile.open(xmlFileName); // create xml

    string newLine; // for storing line string

    if(fin.is_open() && fin2.is_open()&& xmlFile.is_open()) // check is all files are open
    {
        timer = clock(); // save millis start time

        //create start message for the log file "17:30:05.001 Start of file conversion"
        fin2 << logTime() <<" Start of file conversion\n\n"; // log file start
        //create start message for the xml file "<root>\n\<msg-list>\n"
        xmlFile << "<root>\n\<msg-list>\n"; // xml file start


        // Loop for reading lines

        while (getline(fin,newLine))
        {
            string outputMessage =  outputMessageConsrtuct(newLine);

            if (outputMessage[0] != 'E') // if first char of output message is NOT E, redirect to xml file
            {
                //cout <<  outputMessage <<endl;
                xmlFile << "\t<msg>" <<outputMessage<< "</msg>\n" ;
            }
            else if (outputMessage[0] == 'E') // else if first char of output message is E, redirect to log file
            {
                fin2 << logTime() <<" " <<  outputMessage << "\n"<< endl;
            }
        }
        xmlFile << "</msg-list>\n"; // xml message part end

        // xml total and date part

        xmlFile << "<totals> cnt=\""<< cnt <<"\" sum=\""<< totalSum <<"\" ";
        xmlFile << "date = \""<< xmlFileTime() <<"/\"</totals>\n";

        //create end message for the log file  "17:30:06.020 End of conversion"
        fin2 << logTime() <<" End of conversion"; // log file end

        xmlFile << "</root>"; // xml file end

        fin.close(); // close file
        fin2.close();// close file2
        xmlFile.close();// close xmlFile

        // Print success message
        cout << "Files " << logFileName <<" and " << xmlFileName <<" created! " << endl;
    }
    else
    {
        // Print ERROR message
        cout<<"ERROR: file loading error!!!" << endl; // if files are not open cout Error message
    }

    return 0;
}
