#include "PrecompiledHeader.h" //precompiled header
#include "Utilities.h"

std::string OnlineUtilities::len_to_string(int length){
	std::string str="";
	if (length <=0) return str;
	int quotient = (int) length/255;
	int remainder = length % 255;
	for (int i=0; i < quotient; i++) str.push_back((char) 255);
	str.push_back(remainder);
	return str;
}
int OnlineUtilities::string_to_len(std::string str){
	int len = 0;
	for (std::string::iterator i = str.begin(); i!=str.end(); i++){
		len += (int) *i;
	}
	return len;
}
std::vector<std::string> OnlineUtilities::break_string(std::string stream){
	std::vector<std::string> output;
	int index = 0;
	int len = 0;
	int size = (int) stream.size();
	while (index < size){
		int c = (int) stream[index];
		if (c<0) c += 256;
		len += c;
		index += 1;
		if (c < 255){
			output.push_back(stream.substr(index,len));
			index += len;
			len = 0;
		}
	}
	return output;
}
std::string OnlineUtilities::add_string(std::vector<std::string> list){
	std::string output = "";
	for (int i = 0; i < (int) list.size(); i++){
		int len = (int) list[i].size();
		output.append(len_to_string(len));
		output.append(list[i]);
	}
	return output;
}
VertexStructure::Position::Position(GLfloat _x, GLfloat _y, GLfloat _z) {
	x = _x;
	y = _y;
	z = _z;
}
VertexStructure::Position2D::Position2D(GLfloat _x, GLfloat _y) {
	x = _x;
	y = _y;
}
VertexStructure::Color::Color(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a) {
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}
bool ResourceUtilities::extractZipResource(const void* rawData, size_t sizeOfData, std::vector<ResourceUtilities::FileData*>& storageLocation, const std::set<std::string>& fileNameList, wxProgressDialog* progressDialog) {
	std::shared_ptr<wxMemoryInputStream> file(new wxMemoryInputStream(rawData, sizeOfData));
	std::shared_ptr<wxZipInputStream> zip(new wxZipInputStream(*file));
	storageLocation.clear();
	std::shared_ptr<wxZipEntry> entry(nullptr);
	unsigned int count = 1;
	while (entry.reset(zip->GetNextEntry()), entry.get() != NULL) {
		zip->OpenEntry(*entry.get());
		if (zip->CanRead()) {
			ResourceUtilities::FileData* entryData = new ResourceUtilities::FileData(); //File handle containing the raw data, size and name.
			entryData->name = entry->GetName();
			wxFileOffset entrySize = entry->GetSize();
			entryData->size = entrySize;
			entryData->data = new BYTE[entrySize]; //Allocate memory to contain zip entry data
			storageLocation.push_back(entryData);
			zip->Read(entryData->data, entrySize);
			if (zip->LastRead() == entrySize) {
				if (!fileNameList.count(entryData->name)) {
					wxMessageBox(wxString::Format("Invalid entry '%s' in module resource!", entryData->name.c_str()), "Resource Extraction Error!", wxOK | wxICON_ERROR, progressDialog);
					return false;
				}
				if (progressDialog) { //Update the progress dialog if there is any.
					int currentValue = progressDialog->GetValue();
					progressDialog->Update(currentValue + 1, wxString::Format("Extracting resources ... (extracted: %d)", count));
				}
				count++;
			}
			else {
				wxMessageBox(wxString::Format("Failed to read all bytes of entry '%s' in module resource!", entryData->name.c_str()), "Resource Extraction Error!", wxOK | wxICON_ERROR, progressDialog);
				return false;
			}
		}
	}
	return true;
}
bool ResourceUtilities::extractZipResource(const char* pathToZipFile, std::vector<ResourceUtilities::FileData*>& storageLocation, const std::set<std::string>& fileNameList, wxProgressDialog* progressDialog) {
	wxFileInputStream zipFile(pathToZipFile);
	if (!zipFile.IsOk()) {
		wxMessageBox(wxString::Format("Could not locate zip file '%s'.", pathToZipFile), "Resource Loading Error!", wxOK | wxICON_ERROR);
		return false;
	}
	std::shared_ptr<wxZipInputStream> zip(new wxZipInputStream(zipFile));
	storageLocation.clear();
	std::shared_ptr<wxZipEntry> entry(nullptr);
	unsigned int count = 1;
	while (entry.reset(zip->GetNextEntry()), entry.get() != NULL) {
		zip->OpenEntry(*entry.get());
		if (zip->CanRead()) {
			ResourceUtilities::FileData* entryData = new ResourceUtilities::FileData(); //File handle containing the raw data, size and name.
			entryData->name = entry->GetName();
			wxFileOffset entrySize = entry->GetSize();
			entryData->size = entrySize;
			entryData->data = new BYTE[entrySize]; //Allocate memory to contain zip entry data
			storageLocation.push_back(entryData);
			zip->Read(entryData->data, entrySize);
			if (zip->LastRead() == entrySize) {
				if (!fileNameList.count(entryData->name)) {
					wxMessageBox(wxString::Format("Invalid entry '%s' in module resource!", entryData->name.c_str()), "Resource Extraction Error!", wxOK | wxICON_ERROR, progressDialog);
					return false;
				}
				if (progressDialog) { //Update the progress dialog if there is any.
					int currentValue = progressDialog->GetValue();
					progressDialog->Update(currentValue + 1, wxString::Format("Extracting resources ... (extracted: %d)", count));
				}
				count++;
			}
			else {
				wxMessageBox(wxString::Format("Failed to read all bytes of entry '%s' in module resource!", entryData->name.c_str()), "Resource Extraction Error!", wxOK | wxICON_ERROR, progressDialog);
				return false;
			}
		}
	}
	return true;
}
void ResourceUtilities::clearZipStorage(std::vector<ResourceUtilities::FileData*>& storage) {
	for (auto entry : storage) {
		if (entry != nullptr) {
			delete[] entry->data;
			delete entry;
		}
	}
	storage.clear();
}
void ResourceUtilities::clearMeshStorage(std::unordered_map<std::string, MeshData*>& dataSet) {
	for (auto entry : dataSet) {
		delete entry.second;
	}
	dataSet.clear();
}