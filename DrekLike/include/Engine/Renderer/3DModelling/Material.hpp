#pragma once
#include "Engine/Renderer/Shaders/ShaderProgram.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/CubeMap.hpp"
#include "Engine/Math/Matrix44.hpp"

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

class Material
{
public:
	//constructors
	Material();
	Material(ShaderProgram* shaderProg, std::vector<Texture*>& texturesToUse, std::vector<unsigned int>& texBinds);
	Material(const Material&);
	~Material();

	//setters
	void SetProgram(ShaderProgram* shaderProg);
	void SetTextures(std::vector<Texture*>& texturesToUse);
	void SetCubeMap(std::vector<CubeMap*>& CubeMapsToUse);
	void SetTexture(Texture* texToUse, const unsigned int& textureBind, const std::string& name, const unsigned int& index);
	void SetCubeMap(CubeMap* cubeToUse, const unsigned int& cubeMapBind, const std::string& name, const unsigned int& index);
	void AddTexture(Texture* texToUse, const unsigned int& textureBind, const std::string& textureBindName = "");
	void AddCubeMap(CubeMap* texToUse, const unsigned int& textureBind, const std::string& textureBindName = "");
	void SetTextureBindLocations(std::vector<unsigned int>& texBinds) { textureBinds = texBinds; };
	void SetCubeMapBindLocations(std::vector<unsigned int>& cubeBinds) { cubeMapBinds = cubeBinds; };
	void SetMatrices(const Matrix44& model, const Matrix44& view, const Matrix44& projection);

	//getters
	ShaderProgram* GetProgram() const { return meshShaderProg; };
	const std::vector<Texture*> GetTextures() const { return meshTextures; };
	const std::vector<CubeMap*> GetCubeMaps() const { return meshCubeMaps; };
	const std::vector<unsigned int> GetBindLocations() const { return textureBinds; };
	const std::vector<unsigned int> GetCubeMapBindLocations() const { return cubeMapBinds; };
	const std::vector<std::string> GetTextureBindNames() const { return textureNames; };
	const std::vector<std::string> GetCubeMapBindNames() const { return cubeMapNames; };

	//operators
	void ActiveTextures(const unsigned int& samplerID) const;
	void DeactivateTextures() const;
	void DeleteProgram();
private:
	std::vector<unsigned int> textureBinds;
	std::vector<unsigned int> cubeMapBinds;
	std::vector<Texture*> meshTextures;
	std::vector<CubeMap*> meshCubeMaps;
	std::vector<std::string> textureNames;
	std::vector<std::string> cubeMapNames;
	ShaderProgram* meshShaderProg;
};
#endif