#include "Texture.h"

Texture::Texture()
{
	// Initialize to null
	m_targaData = 0;
	m_texture = 0;
	m_textureView = 0;
}

Texture::Texture(const Texture& other) {};
Texture::~Texture() {};

bool Texture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool res;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hRes;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// Load the targa image data
	res = LoadTarga32Bit(filename);
	if (!res)
	{
		return false;
	}

	// Setup the description of the texture.
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture
	hRes = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hRes))
	{
		return false;
	}

	// Set the row pitch of the targa image data
	rowPitch = (m_width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// Setup the shader resource view description
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture
	hRes = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hRes))
	{
		return false;
	}

	// Generate mipmaps for this texture
	deviceContext->GenerateMips(m_textureView);

	// Release the targa image data
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void Texture::Shutdown()
{
	// Release
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_textureView;
}

int Texture::GetWidth()
{
	return m_width;
}


int Texture::GetHeight()
{
	return m_height;
}

//This is our Targa image loading function. 
//Once again note that Targa images are stored upside down and need to be flipped before using. 
//So here we will open the file, read it into an array, and then take that array data and load it into the m_targaData array in the correct order. 
//Note we are purposely only dealing with 32-bit Targa files that have alpha channels, this function will reject Targa's that are saved as 24-bit.
bool Texture::LoadTarga32Bit(char* filename)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;


	// Open the targa file for reading in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	imageSize = m_width * m_height * 4;

	// Allocate memory for the targa image data.
	targaImage = new unsigned char[imageSize];

	// Read in the targa image data.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (m_width * m_height * 4) - (m_width * 4);

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down and also is not in RGBA order.
	for (j = 0; j < m_height; j++)
	{
		for (i = 0; i < m_width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (m_width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;

	return true;
}