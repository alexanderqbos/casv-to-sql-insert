#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

const int MAX_ROWS_INSERTED = 999;

std::vector<std::string> splitLine(std::string line)
{
    std::string rowString = line;
    std::vector<std::string> rowValues;

    do
    {
        int start = 0;
        int end = rowString.find(',');
        
        std::string nLine = rowString.substr(start, end);

        rowString = rowString.substr(end + 1);

        rowValues.push_back(nLine);

    } while (rowString.find(',') != std::string::npos);
    
    rowValues.push_back(rowString);

    return rowValues;
}

int main()
{
    std::string myText;

    std::ifstream MyReadFile("h://input.csv");

    std::vector<std::vector<std::string>> csv;

    // Use a while loop together with the getline() function to read the file line by line
    while (std::getline(MyReadFile, myText)) {
        // Output the text from the file
        csv.push_back(splitLine(myText));
    }

    // Close the file
    MyReadFile.close();

    // write to new file the sql insert code
    std::string tableName = "testTable"; // temp name till args implemented

    // set table name to uppercase
    std::transform(tableName.begin(), tableName.end(), tableName.begin(), ::toupper);

    // create new .sql file for insert
    std::ofstream outputSQL("h://" + tableName + ".sql");

    outputSQL << "INSERT INTO " + tableName + " VALUES\n";

    // use line number to limit each insert to 1000 records (limit of multi insert)
    int lineNum = 0;
    int rowCount = static_cast<int>(csv.size());

    for (std::vector<std::string> vecString : csv)
    {
        int i = 1; // int i is used to control where to ',' go in code
        outputSQL << "(";
        for (std::string val : vecString)
        {
            outputSQL << val;
            if (i != vecString.size())
            {
                outputSQL << ",";
            }
            i++;
        }

        lineNum++;

        std::cout << lineNum << "==" << rowCount << std::endl;

        if (lineNum == rowCount)
        {
            // End of rows, end file
            outputSQL << ");\n";
        }
        else if (lineNum == MAX_ROWS_INSERTED)
        {
            // End of insert size, create new insert statement
            outputSQL << ");\n";
            outputSQL << "\nINSERT INTO " + tableName + " VALUES\n";
            rowCount = rowCount - lineNum;
            lineNum = 0;
        }
        else {
            outputSQL << "),\n";
        }
    }

    outputSQL.close();
}