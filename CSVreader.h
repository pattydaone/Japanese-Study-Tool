/*
    This code is taken from stack overflow user Loki Astari (from the below link) with
    some slight modification by me for my use case (you can see who did what based on the bracket placement).
    Link: https://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c
*/

#ifndef CSVREADER_H
#define CSVREADER_H

#include <iostream>
#include <vector>
#include <string>

class CSVRow
{
    public:
        std::string operator[](std::size_t index) const
        {
            return std::string(&m_line[m_data[index] + 1], m_data[index + 1] -  (m_data[index] + 1));
        }

        std::size_t size() const
        {
            return m_data.size() - 1;
        }

        void readNextRow(std::istream& str)
        {
            std::getline(str, m_line);

            q_data.clear();
            std::string::size_type q_pos = 0;
            while ((q_pos = m_line.find('"', q_pos)) != std::string::npos) {
                q_data.emplace_back(q_pos);
                ++q_pos;
            }

            m_data.clear();
            m_data.emplace_back(-1);
            std::string::size_type pos = 0;
            while((pos = m_line.find(',', pos)) != std::string::npos)
            {
                if (q_data.size() > 0) {

                    if (pos < q_data[0] || pos > q_data[1]) {
                        m_data.emplace_back(pos);
                    }
                    ++pos;
                }
                else {
                    m_data.emplace_back(pos);
                    ++pos;
                }
            }
            // This checks for a trailing comma with no data after it.
            pos = m_line.size();
            m_data.emplace_back(pos);
        }
    private:
        std::string m_line;
        std::vector<int> m_data;
        std::vector<std::string::size_type> q_data;
};

inline std::istream& operator>>(std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}

#endif // CSVREADER_H
