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

int main(int argc, char* argv[])
{
    std::string tableName;
    std::string filePath;

    bool headers;

    // Handle arguments, and give informative errors
    if (argc != 4)
    {
        // not enough arguments
        std::cerr << "Expected arguments, <tableName> <inputFile> (-headers || -noHeaders)\n\ttableName, inputfile, and either headers or noHeaders if inputFile has headers at the top\n";
        return -1;
    }
    else {

        tableName = argv[1]; //set tableName
        filePath = argv[2]; //set filePath

        std::string headerString = argv[3]; //evaluate if we use headers or not!
        if (std::string("-headers").compare(headerString))
        {
            headers = true;
        }
        else if (std::string("-noHeaders").compare(headerString))
        {
            headers = false;
        }
        else {
            std::cerr << "Expected either -headers or -noHeaders as thrid argument!";
            return -1;
        }
    }

    std::string myText;

    std::ifstream inputFileReader("h://input.csv");

    std::vector<std::vector<std::string>> csv;

    // Use a while loop together with the getline() function to read the file line by line
    while (std::getline(inputFileReader, myText)) {
        // Output the text from the file
        csv.push_back(splitLine(myText));
    }

    // Close the file
    inputFileReader.close();

    // set table name to uppercase
    std::transform(tableName.begin(), tableName.end(), tableName.begin(), ::toupper);

    // create new .sql file for insert
    std::ofstream outputSQL("h://" + tableName + ".sql");

    // configure insert to include column headers depending on arguments
    std::string insertHeader;

    if (!headers)
    {
        // -headers passed in
        insertHeader = "INSERT INTO \n\t" + tableName + "(";
        std::vector<std::string> header = csv.at(0); //first row (should be column names)
        for (std::string val : header)
        {
            insertHeader += val + ",";
        }
        insertHeader = insertHeader.substr(0, insertHeader.size() - 1);
        insertHeader += ")\nVALUES\n";

        csv.erase(csv.begin()); // remove first row to avoid duplicating column names in first insert.
    }
    else {
        // -noHeaders passed in
        insertHeader = "INSERT INTO \n\t" + tableName + "\nVALUES\n";
    }
    
    outputSQL << insertHeader; // print insert statement head in.

    // use line number to limit each insert to 1000 records (limit of multi insert)
    int lineNum = 0;
    int rowCount = static_cast<int>(csv.size());

    for (std::vector<std::string> vecString : csv)
    {
        int i = 1; // int i is used to control where to ',' go in code
        outputSQL << "\t(";
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

        if (lineNum == rowCount)
        {
            // End of rows, end file
            outputSQL << ");\n";
        }
        else if (lineNum == MAX_ROWS_INSERTED)
        {
            // End of insert size, create new insert statement
            outputSQL << ");\n";
            outputSQL << "\n" << insertHeader;
            rowCount = rowCount - lineNum;
            lineNum = 0;
        }
        else {
            outputSQL << "),\n";
        }
    }

    outputSQL.close();
}