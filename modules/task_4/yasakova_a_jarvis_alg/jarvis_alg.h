// Copyright 2020 Yasakova Anastasia
#ifndef MODULES_TASK_4_YASAKOVA_A_JARVIS_ALG_JARVIS_ALG_H_
#define MODULES_TASK_4_YASAKOVA_A_JARVIS_ALG_JARVIS_ALG_H_

#include <utility>
#include <vector>


std::vector < std::pair<int, int>> GetRandomPoints(int n);

std::vector < std::pair<int, int>> std_JarvisAlg(const std::vector<std::pair<int, int>>& points);
std::pair<int, int> std_FindFirstPoint(const std::vector<std::pair<int, int>>& points);
std::pair<int, int> std_FindSecondPoint(const std::vector<std::pair<int, int>>& points, std::pair<int, int> tmp);

std::vector < std::pair<int, int>> seq_JarvisAlg(const std::vector<std::pair<int, int>>& points);
std::pair<int, int> seq_FindFirstPoint(const std::vector<std::pair<int, int>>& points);
std::pair<int, int> seq_FindSecondPoint(const std::vector<std::pair<int, int>>& points, std::pair<int, int> tmp);

double CountCos(std::pair<int, int> a, std::pair<int, int> b, std::pair<int, int> c);
double distance(std::pair<int, int> a, std::pair<int, int> b);

#endif   // MODULES_TASK_4_YASAKOVA_A_JARVIS_ALG_JARVIS_ALG_H_
