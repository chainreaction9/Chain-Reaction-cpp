
#include "PrecompiledHeader.h" //precompiled header
#include "MainGame.h"
#include "AppGUIFrame.h"
#include "MeshGenerator.h"
#include "Text.h"
#include "SoundSystem.h"
#include "Utilities.h"

#if defined (_WIN32) //Include resource header only in Windows environment.
#include "Resource.h"
#else //Define the path to the zip resource.
constexpr auto _PATH_TO_THE_ZIP_RESOURCE_ = "./Resources/resource.zip"
#endif //!_WIN32
/*
* @brief A class derived from wxApp which serves as the main entry point of the wxWidget application.
*/
class wxAppInstance: public wxApp{
public:
	/*
	* @brief Main entry point of the application (wxAppInstance).
	*/
	virtual bool OnInit(); 
	/*
	* @brief Default destructor of application (wxAppInstance). Handles deletion of dynamically allocated memories.
	*/
	virtual int OnExit() wxOVERRIDE; 
private:
	std::shared_ptr<wxMenu> _popupMenu = nullptr;
	std::shared_ptr<MainGame> _offlineGameObject = nullptr; //Pointer to the object associated with an offline game.
	std::shared_ptr<GameUtilities::GameState> _gameInformation = nullptr; //Pointer to the object containing current game state.
	std::shared_ptr<ResourceUtilities::ResourceData> _resourceData = nullptr; //Pointer to resources containing e.g., text object created from alphabet mesh data, textures, etc.
	std::shared_ptr<Text> _textObject = nullptr; //Pointer to the text object which is used for rendering 3D text.
	std::unordered_map<std::string, MeshData*> _characterSet; //Map between name of the 26 alphabets and its mesh data. This is required as an argument for creating a Text object.
};
//********************************************************* Source code ****************************

bool wxAppInstance::OnInit() {
	//******* Initialize the main GUI frame instance.
	AppGUIFrame* mainFrame = new AppGUIFrame("Chain Reaction", wxDefaultPosition, wxDefaultSize, wxSize(630, 600));

#if defined (_WIN32) //Load icon and zip file from resource('.rc') file. Applicable to windows environment only.
	//************* Set icon property of the application ******************
	//Locate and load icon file from the resource. Resource ID (IDI_ICON1) for the icon is defined in "resource.h" header.
	HICON iconResource = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1));
	if (iconResource == NULL) {
		wxMessageBox(wxT("Failed to retrieve icon resource data"), wxT("Resource Handling Error"), wxOK | wxICON_ERROR);
		return false;
	}
	wxIcon icon;
	icon.CreateFromHICON(iconResource);
	mainFrame->SetIcon(icon); //Set icon property of the application frame.
	//***** Locate the resource file (zip) in memory. Resource ID (IDR_ZIP1) for the zip is defined in "resource.h" header.
	HANDLE zip_resource_info = FindResource(GetModuleHandle(0), MAKEINTRESOURCE(IDR_ZIP1), TEXT("ZIP"));
	if (zip_resource_info == NULL) { //Handle error with locating zip resource
		wxMessageBox(wxT("Failed to retrieve resource information"), wxT("Resource Handling Error"), wxOK | wxICON_ERROR);
		return false;
	}
	//***** Load the zip resource on heap.
	HANDLE zipResource = LoadResource(GetModuleHandle(0), (HRSRC)zip_resource_info);
	if (zipResource == NULL) { //Handle error with loading zip resource.
		wxMessageBox(wxT("Failed to retrieve resource data"), wxT("Resource Handling Error"), wxOK | wxICON_ERROR);
		//delete menu;
		return false;
	}
	//***** Retrive size of the zip resource.
	size_t zipSize = SizeofResource(GetModuleHandle(0), (HRSRC)zip_resource_info);
#endif //!_WIN32
	//***** Set of file names that are contained in the zip resource. It is included in the code as a failsafe.
	std::set<std::string> fileList = { "A.obj", "B.obj", "C.obj", "D.obj",
									"E.obj", "F.obj", "G.obj", "H.obj",
									"I.obj", "J.obj", "K.obj", "L.obj",
									"M.obj", "N.obj", "O.obj", "P.obj",
									"Q.obj", "R.obj", "S.obj", "T.obj", 
									"U.obj", "V.obj", "W.obj", "X.obj",
									"Y.obj", "Z.obj", "audio.wav", "fragment-shader-gridlines.txt",
									"vertex-shader-gridlines.txt", "vertex-shader-orbs.txt", "fragment-shader-orbs.txt", "vertex-shader-texts.txt",
									"fragment-shader-texts.txt", "vertex-shader-particles.txt", "fragment-shader-particles.txt", "alphabet.mtl" };
	unsigned int fileCount = fileList.size(); //dummy variable to keep track of progress and update progress dialog.
	int totalNumberOfTasks = 2 * (1 + fileCount) + 3;
	//*******************************************************************
	this->_popupMenu.reset(new wxMenu()); //used as a wxPopupMenu.
	mainFrame->info_menu = this->_popupMenu.get(); //Attach the popmenu to the main frame.
	mainFrame->Show(true);
	mainFrame->Iconize(false);
	mainFrame->canvas_panel->Update();
	mainFrame->canvas_panel->ApplyDefaultCameraSetup();
	mainFrame->Update();
	//**** Progress dialog to display progress on resource extraction, loading, initialization, etc.
	shared_ptr<wxProgressDialog> progressDialog(new wxProgressDialog("Wait!", "Extracting resources ...", totalNumberOfTasks, mainFrame, wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME));
	progressDialog->Update(1, "Extracting resources ...");
	std::vector<ResourceUtilities::FileData*> allResourceDataContainer;//Storage location for zip extraction. It is a vector containing the file data (e.g., file name, raw binary data, size of data) of each entry in the zip.
	bool isResourceExtracted = false;
#if defined (_WIN32)
	//***** Extract zip file loaded from resource('.rc') file **
	isResourceExtracted = ResourceUtilities::extractZipResource(zipResource, zipSize, allResourceDataContainer, fileList, progressDialog.get());
	FreeResource(zipResource); //Zip extraction completed. Free zip resource from heap memory.
#else
	isResourceExtracted = ResourceUtilities::extractZipResource(_PATH_TO_THE_ZIP_RESOURCE_, allResourceDataContainer, fileList, progressDialog.get());
#endif //!_WIN32
	if (!isResourceExtracted) { //Handle zip extraction error
		ResourceUtilities::clearZipStorage(allResourceDataContainer);
		progressDialog->Destroy();
		return false;
	}
	ResourceUtilities::FileData* audioData = nullptr; //reference to the audio data in resource
	std::unordered_map<std::string, std::string> shaderData; //An unordered map that points each shader file name to its raw string data.
	std::unordered_map<std::string, objl::Material> materialList; //storage location of extracted materials from '.mtl' data.
	unsigned int currentValue = progressDialog->GetValue();
	progressDialog->Update(currentValue + 1, "Loading resources ...");
	//***** Convert raw resource data into MeshData/AudioData/MaterialData and upload in GPU.
	for (unsigned int i = 0; i < allResourceDataContainer.size(); i++) {
		ResourceUtilities::FileData* entryData = allResourceDataContainer[i];
		if (!entryData) continue;
		std::string fileName(entryData->name);
		std::string fileExtension = fileName.size() > 4 ? fileName.substr(fileName.size() - 4) : "";
		if (!fileList.count(fileName)) { // Handle error as name of the file is not in the expected list of file names.
			wxMessageBox(wxString::Format("Failed to load resource '%s'.", fileName.c_str()), "Resource Loading Error!", wxOK | wxICON_ERROR, progressDialog.get());
			ResourceUtilities::clearMeshStorage(this->_characterSet); //Free memory allocated to create Mesh (characters) objects.
			ResourceUtilities::clearZipStorage(allResourceDataContainer); //Free all raw data that were extracted from the zip resource.
			progressDialog->Destroy();
			return false;
		}
		else if (!fileExtension.compare(".obj")) { //The data is of a 3D object file
			//Convert raw data into MeshData and upload in GPU.
			MeshData* mesh = new MeshData();
			bool meshLoadSuccess = mesh->LoadFromMemory((BYTE*) entryData->data, entryData->size);
			std::string meshName = fileName.substr(0,fileName.size() - 4); //Get the file name without extension.
			if (!meshLoadSuccess) { //MeshData conversion failed. Handle error.
				wxMessageBox(wxT("Could not parse 'obj' data! Mesh loading failed."), wxT("Mesh loading Error"), wxOK | wxICON_ERROR, progressDialog.get());
				//********** Delete previously allocated memory ****************
				ResourceUtilities::clearMeshStorage(this->_characterSet); //Free memory allocated to create Mesh (characters) objects.
				ResourceUtilities::clearZipStorage(allResourceDataContainer); //Free all raw data that were extracted from the zip resource.
				progressDialog->Destroy();
				delete mesh;
				//*************************************************************
				return false;
			}
			this->_characterSet[meshName] = mesh; //MeshData conversion successful. Store mesh object at the correct index of charset array.
		}
		else if (!fileExtension.compare(".wav")) { //the data is of an audio file
			audioData = entryData; //set the pointer of audioData to the .wav resource.
		}
		else if (!fileExtension.compare(".mtl")) {
			objl::Loader::LoadMaterialsFromMemory((BYTE*)entryData->data, entryData->size, materialList);
		}
		else if (!fileExtension.compare(".txt")) {
			shaderData[fileName] = std::string(entryData->data, entryData->data + entryData->size);
		}
		else {
			wxMessageBox(wxString::Format("Unexpected entry '%s' in resource data.", fileName.c_str()), "Resource Loading Error!", wxOK | wxICON_ERROR, progressDialog.get());
			ResourceUtilities::clearMeshStorage(this->_characterSet);
			ResourceUtilities::clearZipStorage(allResourceDataContainer);
			progressDialog->Destroy();
			return false;
		}
		currentValue = progressDialog->GetValue();
		progressDialog->Update(currentValue + 1, wxString::Format("Loading resources ... (loaded: %d) ", i + 1));
	}
	//********************************************************************************************
	this->_resourceData.reset(new ResourceUtilities::ResourceData()); //resource data containing text object created from MeshData and default material.
	this->_textObject.reset(new Text(1.2f, 1.0f, 2.1f, this->_characterSet)); //text object generated from MeshData.
	this->_resourceData->textObject = this->_textObject.get();
	this->_resourceData->particleShaderVertexSource = shaderData["vertex-shader-particles.txt"].c_str();
	this->_resourceData->particleShaderFragmentSource = shaderData["fragment-shader-particles.txt"].c_str();
	this->_resourceData->textShaderVertexSource = shaderData["vertex-shader-texts.txt"].c_str();
	this->_resourceData->textShaderFragmentSource = shaderData["fragment-shader-texts.txt"].c_str();
	currentValue = progressDialog->GetValue();
	progressDialog->Update(currentValue + 1, "Creating game object ... ");
	if (audioData==nullptr) { //If audio data was not found while extracting resources, handle error.
		wxMessageBox("Could not locate audio ('.wav') data while extracting resources", "Resource Loading Error!", wxOK | wxICON_ERROR, progressDialog.get());
		//********** Delete previously allocated memory ****************
		ResourceUtilities::clearZipStorage(allResourceDataContainer); //Free all raw data that were extracted from the zip resource.
		ResourceUtilities::clearMeshStorage(this->_characterSet); //Free memory allocated to create Mesh (characters) objects.
		progressDialog->Destroy();
		//*************************************************************
		return false;
	}
	else if (materialList.empty()) { //If material data was not extracted from resources, handle error.
		wxMessageBox("Could not parse material ('.mtl') data from memory resources", "Resource Loading Error!", wxOK | wxICON_ERROR, progressDialog.get());
		//********** Delete previously allocated memory ****************
		ResourceUtilities::clearZipStorage(allResourceDataContainer); //Free all raw data that were extracted from the zip resource.
		ResourceUtilities::clearMeshStorage(this->_characterSet); //Free memory allocated to create Mesh (characters) objects.
		progressDialog->Destroy();
		//*************************************************************
		return false;
	}
	else{
		bool isAudioSystemInitialized = SoundSystem::initSystem(); //Initialize default audio system.
		if (!isAudioSystemInitialized) { //Handle error if initialization fails.
			wxMessageBox("Failed to initialize default audio system.", "Audio system failure!", wxOK | wxICON_ERROR, progressDialog.get());
			//********** Delete previously allocated memory ************
			ResourceUtilities::clearZipStorage(allResourceDataContainer); //Free all raw data that were extracted from the zip resource.
			ResourceUtilities::clearMeshStorage(this->_characterSet); //Free memory allocated to create Mesh (characters) objects.
			progressDialog->Destroy();
			//************************************************************
			return false;
		}
		for (auto& entry : this->_characterSet){ // Update default material properties of each mesh from materiaList.
			entry.second->updateDefaultMaterials(materialList);
		}
		this->_resourceData->defaultSphereMaterial.name = "Plastic"; // use the Plastic material defined in material resource (alphabet.mtl) as a default for the brownian particles.
		for (const auto& material : materialList) {
			if (!(material.first.compare(this->_resourceData->defaultSphereMaterial.name))) {
				this->_resourceData->defaultSphereMaterial = material.second; //stores the extracted Plastic material data to the AppFrame as a resource.
				break;
			}
		}
		mainFrame->canvas_panel->SetResourceData(this->_resourceData.get()); //Pass the object containing resource data (text, material, etc.) to the OpenGL canvas.

		this->_offlineGameObject.reset(new MainGame(audioData->data, audioData->size, shaderData["vertex-shader-gridlines.txt"].c_str(), shaderData["fragment-shader-gridlines.txt"].c_str(), shaderData["vertex-shader-orbs.txt"].c_str(), shaderData["fragment-shader-orbs.txt"].c_str())); //Create offline game object.
		
		this->_offlineGameObject->setFrame(mainFrame); //Inform game object about the current GUI frame to maintain back and forth communication.
		this->_gameInformation.reset(new GameUtilities::GameState()); //Create object containing runtime game information
		this->_gameInformation->mouse_data_processed = 1;
		this->_gameInformation->game = this->_offlineGameObject.get(); //Attach offline game object
		this->_gameInformation->online_game = nullptr; //Attach online game object
		currentValue = progressDialog->GetValue();
		progressDialog->Update(currentValue + 1, "All resources were loaded successfully! Initialising video system ...");
		mainFrame->canvas_panel->SetClientData(this->_gameInformation.get()); //Pass the object containing game info to the OpenGL canvas.
		ResourceUtilities::clearZipStorage(allResourceDataContainer); //Free all raw data that were extracted from the zip resource.
		currentValue = progressDialog->GetValue();
		progressDialog->Update(currentValue + 1); //Final update which automatically destroys the progress dialog.
	}
	return true;
}
int wxAppInstance::OnExit() {
	ResourceUtilities::clearMeshStorage(this->_characterSet); //Free memory allocated to create Mesh (characters) objects.
	SoundSystem::shutDown(); // Free memory allocated to create audio objects.
	return wxApp::OnExit();
}
//Enrty point of wxWidget application.
IMPLEMENT_APP(wxAppInstance);