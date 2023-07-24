#include "Input.h"

Input::Input() {};
Input::Input(const Input& other) {};
Input::~Input() {};

void Input::Initialize()
{
	int i;
	// Initialize all keys to released
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}
	return;
}

void Input::KeyDown(unsigned int input)
{
	// Save key press
	m_keys[input] = true;
	return;
}

void Input::KeyUp(unsigned int input)
{
	// Clear key release
	m_keys[input] = false;
	return;
}

bool Input::IsKeyDown(unsigned int key)
{
	// Return key state
	return m_keys[key];
}