#pragma once

#ifndef FEATUREFACTORY_HPP
#define FEATUREFACTORY_HPP
#include "Game/Generators/Generator.hpp"
#include "Game/Features/FeatureTemplate.hpp"
#include "Game/Generators/EnvironmentGenerationProcess.hpp"

class FeatureFactory : public Generator
{
private:
	FeatureTemplate m_template;
	FeatureType m_type;
	std::string m_name;
	static std::map<std::string, FeatureFactory*> s_FeatureFactoryMaps[NUM_OF_FEATURE_TYPES];
	static std::vector<std::string> s_FeatureNames[NUM_OF_FEATURE_TYPES];
public:
	//Constructors
	FeatureFactory();
	FeatureFactory(EnvironmentGenerationProcess* data);
	~FeatureFactory();

	//Setters
	void SetTemplate(const FeatureTemplate& templ);
	void SetName(const std::string& name);

	//Getters
	const FeatureTemplate GetTemplate() const;
	const std::string GetName() const;
	const FeatureType GetType() const;


	//Static Methods
	static Feature* GenerateFeatureByType(const FeatureType& type);
	static const int GenerateNFeatures(const int& n, SpriteSheet* m_spriteSheet);
	static EnvironmentGenerationProcess* FeatureGenerationProcCreateFunc(const XMLNode& node);
	static Generator* FeatureGeneratorCreationFunc(const std::string& name);
	static void LoadAllFeatures();
	static void ClearAllFeatureFactories();


};
#endif