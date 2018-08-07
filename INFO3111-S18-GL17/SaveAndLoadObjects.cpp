#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "globalStuff.h"
#include "SaveAndLoadObjects.h"

void save_to_file() {
	std::ofstream outputFile("output.txt");
	if (outputFile.is_open()) {
		outputFile << "OBJECTS\n";
		outputFile << "NAME\tPOS(XYZ)\tCOLOUR(RGBA)\tORIENTATION(XYZ)\tSCALE\tWIREFRAME\tLIGHTOBJECT\n";
		for (int i = 0; i < ::g_vec_pMeshObjects.size(); i++) {
			outputFile << ::g_vec_pMeshObjects[i]->meshName << " ";
			outputFile << ::g_vec_pMeshObjects[i]->pos.x << " " << ::g_vec_pMeshObjects[i]->pos.y << " " << ::g_vec_pMeshObjects[i]->pos.z << " ";
			outputFile << ::g_vec_pMeshObjects[i]->colour.r << " " << ::g_vec_pMeshObjects[i]->colour.g << " " << ::g_vec_pMeshObjects[i]->colour.b << " " << ::g_vec_pMeshObjects[i]->colour.a << " ";
			outputFile << ::g_vec_pMeshObjects[i]->orientation.x << " " << ::g_vec_pMeshObjects[i]->orientation.y << " " << ::g_vec_pMeshObjects[i]->orientation.z << " ";
			outputFile << ::g_vec_pMeshObjects[i]->scale << " ";
			outputFile << ::g_vec_pMeshObjects[i]->isWireframe << " ";
			outputFile << ::g_vec_pMeshObjects[i]->bDontLightObject << "\n";
		}

		outputFile << "\nLIGHTS\n";
		outputFile << "POS(XYZ)\tCOLOUR(RGBA)\tCONST\tLINEAR\tQUAD\n";
		for (int i = 0; i < ::g_vecLights.size(); i++) {
			outputFile << ::g_vecLights[i].position.x << " " << ::g_vecLights[i].position.y << " " << ::g_vecLights[i].position.z << " ";
			outputFile << ::g_vecLights[i].diffuseColour.r << " " << ::g_vecLights[i].diffuseColour.g << " " << ::g_vecLights[i].diffuseColour.b << " " << ::g_vecLights[i].diffuseColour.a << " ";
			outputFile << ::g_vecLights[i].attenConst << " ";
			outputFile << ::g_vecLights[i].attenLinear << " ";
			outputFile << ::g_vecLights[i].attenQuad << "\n";
		}
		outputFile.close();
		std::cout << "Successfuly saved file!" << std::endl;
	}
	else {
		std::cout << "Could not open output file!" << std::endl;
	}
}

void skip_lines(std::ifstream& pStream, size_t pLines)
{
	std::string s;
	for (; pLines; --pLines)
		std::getline(pStream, s);
}

void read_from_file() {
	std::ifstream inputFile("output.txt");
	if (inputFile.is_open()) {
		//Skip the first two lines
		skip_lines(inputFile, 2);
		for (int i = 0; i < ::g_vec_pMeshObjects.size(); i++) {
			inputFile >> ::g_vec_pMeshObjects[i]->meshName;
			inputFile >> ::g_vec_pMeshObjects[i]->pos.x >> ::g_vec_pMeshObjects[i]->pos.y >> ::g_vec_pMeshObjects[i]->pos.z;
			inputFile >> ::g_vec_pMeshObjects[i]->colour.r >> ::g_vec_pMeshObjects[i]->colour.g >> ::g_vec_pMeshObjects[i]->colour.b >> ::g_vec_pMeshObjects[i]->colour.a;
			inputFile >> ::g_vec_pMeshObjects[i]->orientation.x >> ::g_vec_pMeshObjects[i]->orientation.y >> ::g_vec_pMeshObjects[i]->orientation.z;
			inputFile >> ::g_vec_pMeshObjects[i]->scale;
			inputFile >> ::g_vec_pMeshObjects[i]->isWireframe;
			inputFile >> ::g_vec_pMeshObjects[i]->bDontLightObject;
		}
		//Skip the next 3
		skip_lines(inputFile, 3);
		for (int i = 0; i < ::g_vecLights.size(); i++) {
			inputFile >> ::g_vecLights[i].position.x >> ::g_vecLights[i].position.y >> ::g_vecLights[i].position.z;
			inputFile >> ::g_vecLights[i].diffuseColour.r >> ::g_vecLights[i].diffuseColour.g >> ::g_vecLights[i].diffuseColour.b >> ::g_vecLights[i].diffuseColour.a;
			inputFile >> ::g_vecLights[i].attenConst;
			inputFile >> ::g_vecLights[i].attenLinear;
			inputFile >> ::g_vecLights[i].attenQuad;
		}
		inputFile.close();
		std::cout << "Input file successfuly loaded" << std::endl;
	}
	else {
		std::cout << "Could not open input file!" << std::endl;
	}
}