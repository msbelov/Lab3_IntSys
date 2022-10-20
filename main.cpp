#include <iostream>
#include <vector>
#include "tinyxml2.h"
#include <string>
#include<algorithm>
#include <cmath>

using namespace std;
using namespace tinyxml2;

class Lab3 {
public:
    vector<vector<float>> cord;
    vector<vector<float>> cord_dif;
    vector<vector<float>> cov_matrix;
    vector<vector<float>> cov_m;
    vector<vector<float>> core;
    float final{};
};


void inverted_matrix(vector<vector<float>> &M) {
    double t;
    vector<vector<float>> E(M.size(), vector<float>(M.size(), 0.0));

    for (int i = 0; i < M.size(); i++)
        for (int j = 0; j < M.size(); j++) {
            if (i == j) E[i][j] = 1.0;
            M[i][j] += E[i][j];
        }


    for (int a = 0; a < M.size(); a++)
    {

        t = M[a][a];

        for (int b = a + 1; b < M.size(); b++)
        {
            M[a][b] = M[a][b] / t;
        }

        for (int b = 0; b < M.size();b++) {
            E[a][b] = E[a][b] / t;
        }

        M[a][a] /= t;

        if (a > 0) {
            for (int i = 0;i < a;i++) {
                for (int j = 0;j < M.size();j++) {
                    E[i][j] = E[i][j] - E[a][j] * M[i][a];
                }
                for (int j = M.size() - 1;j >= a;j--) {
                    M[i][j] = M[i][j] - M[a][j] * M[i][a];
                }
            }
        }
        for (int i = a + 1;i < M.size();i++) {
            for (int j = 0;j < M.size();j++) {
                E[i][j] = E[i][j] - E[a][j] * M[i][a];
            }
            for (int j = M.size() - 1;j >= a;j--) {
                M[i][j] = M[i][j] - M[a][j] * M[i][a];
            }
        }

    }

    for (int i = 0; i < M.size(); i++) {
        for (int b = 0; b < M.size(); b++) {
            M[i][b] = E[i][b];
        }
    }
}

class Loader {
public:
    std::vector<std::vector<std::vector<int>>> examples;
    std::vector<string> classes;
    std::vector<std::vector<std::vector<int>>> tasks;

    bool load_instance(const char *fileName) {
        XMLDocument doc;
        if (doc.LoadFile(fileName) != XMLError::XML_SUCCESS) {
            std::cout << "Error openning input XML file." << std::endl;
            return false;
        }
        XMLElement *root;
        root = doc.FirstChildElement("root");
        XMLElement *objects = root->FirstChildElement("examples");
        for (auto object = objects->FirstChildElement("object"); object; object = object->NextSiblingElement(
                "object")) {
            std::vector<std::vector<int>> example;
            classes.emplace_back(object->Attribute("class"));
            for (auto row = object->FirstChildElement("row"); row; row = row->NextSiblingElement("row")) {
                std::vector<int> line;
                std::string values = row->GetText();
                for (char value: values) {
                    if (value == '1')
                        line.push_back(1);
                    else if (value == '0')
                        line.push_back(0);
                }
                example.push_back(line);
            }
            examples.push_back(example);
        }
        XMLElement *task = root->FirstChildElement("tasks");
        for (auto object = task->FirstChildElement(); object; object = object->NextSiblingElement("object")) {
            std::vector<std::vector<int>> example;
            for (auto row = object->FirstChildElement("row"); row; row = row->NextSiblingElement("row")) {
                std::vector<int> line;
                std::string values = row->GetText();
                for (char value: values) {
                    if (value == '1')
                        line.push_back(1);
                    else if (value == '0')
                        line.push_back(0);
                }
                example.push_back(line);
            }
            tasks.push_back(example);
        }
        return true;
    }

    void printexamples() {
        for (int i = 0; i < examples.size(); i++) {
            std::cout << "\nObject " << i + 1 << " class=" << classes[i] << "\n";
            for(int j=0; j<examples[i].size(); j++) {
                for (int k = 0; k < examples[i][j].size(); k++) {
                    if (examples[i][j][k] == 0) std::cout << " ";
                    else std::cout << "0";
                }
                std::cout << "\n";
            }
        }
    }

    void printtasks(int i, const string &Lab3) {
        std::cout << "\nTask " << i + 1<< " expected class = " << Lab3 <<"\n";
        for (int j = 0; j < tasks[i].size(); j++) {
            for (int k = 0; k < tasks[i][j].size(); k++)
                if (tasks[i][j][k] == 0) std::cout << " ";
                else std::cout << "0";
            std::cout << "\n";
        }

    }
};

int main(int argc, char *argv[]) //argc - argumnet counter, argv - argument values
{
    for (int i = 0; i < argc; i++)
        std::cout << argv[i] << "\n";
    if (argc < 2) {
        std::cout << "Name of the input XML file is not specified." << std::endl;
        return 1;
    }

    Loader loader;
    loader.load_instance(argv[1]);
    loader.printexamples();
    int classes_num = 4;
    int samples_num = 4;
    int tasks_num = 4;
    int row = 10, col = 10;

    vector<vector<Lab3>> field(classes_num, vector<Lab3>(samples_num));

    for (int n = 0; n < classes_num; n++) {
        for (int j = 0; j < samples_num; j++) {
            field[n][j].cord.resize(row);
            field[n][j].core.resize(col);
            field[n][j].cov_matrix.resize(row * col);
            field[n][j].cord_dif.resize(row);
            field[n][j].cov_m.resize(row);
            field[n][j].final = 0;

            for (int i = 0; i < row; i++) {
                field[n][j].cord[i].resize(col);
                field[n][j].core[i].resize(col);
                field[n][j].cord_dif[i].resize(col);
                field[n][j].cov_m[i].resize(col);
            }
            for (int i = 0; i < row * col; i++) {
                field[n][j].cov_matrix[i].resize(row * col);

            }
        }
    }


    for (int step = 0; step < classes_num; step++) {
        for (int obr = 0; obr < samples_num; obr++) {
            for (int i = 0; i < row; i++) {
                for (int j = 0; j < col; j++) {
                    field[step][obr].cord[i][j] = loader.examples[step * classes_num + obr][i][j];
                }
            }
        }
    }


    for (int step = 0; step < classes_num; step++) {
        cout << "Cores object number " << step + 1 << " :\n";
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                for (int obr = 0; obr < samples_num; obr++) {
                    field[step][0].core[i][j] += field[step][obr].cord[i][j];
                }
                field[step][0].core[i][j] /= (row * col);
                cout << field[step][0].core[i][j] << "\t";
            }
            cout << endl;
        }
        cout<<"___________________________________________________________"<<endl;
    }


    for (int step = 0; step < classes_num; step++) {
        for (int i = 0; i < row * col; i++)
            for (int j = 0; j < row * col; j++) {
                for (int obr = 0; obr < samples_num; obr++) {
                    field[step][0].cov_matrix[i][j] +=
                            (field[step][obr].cord[i / row][i % row] - field[step][0].core[i / row][i % row]) *
                            (field[step][0].cord[j / col][j % col] - field[step][0].core[j / col][j % col]);
                }
                field[step][0].cov_matrix[i][j] /= (row * col - 1);
            }


        inverted_matrix(field[step][0].cov_matrix);
    }



    for (int st = 0; st < tasks_num; st++) {
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                for (int step = 0; step < classes_num; step++)
                    field[step][0].cord_dif[i][j] = loader.tasks[st][i][j] - field[step][0].core[i][j];
            }
        }

        for (int step = 0; step < classes_num; step++) {
            for (auto &i: field[step][0].cov_m) {
                for (float &j: i) {
                    j = 0;
                }
            }
        }


        for (int i = 0; i < row * col; i++) {
            for (int j = 0; j < row * col; j++) {
                for (int step = 0; step < classes_num; step++) {
                    field[step][0].cov_m[i / col][i % col] += (field[step][0].cord_dif[j / col][j % col] *
                                                                 field[step][0].cov_matrix[i][j]);
                }
            }
        }


        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                for (int step = 0; step < classes_num; step++) {
                    field[step][0].final += (field[step][0].cov_m[i][j] * field[step][0].cord_dif[i][j]);
                }
            }
        }

        vector<pair<float, int>> best;
        for (int step = 0;step < classes_num;step++) {
            field[step][0].final = sqrt(field[step][0].final);
            best.push_back(make_pair(field[step][0].final, step));
        }
        sort(best.begin(), best.end());

        loader.printtasks(st, loader.classes[best[0].second * 4]);
        for (int step = 0;step < classes_num;step++) {
            cout << "Distance to " << loader.classes[step * 4] << " : "<< field[step][0].final << endl;
        }
        cout << "____________________________________\n";
    }

    return 0;
}