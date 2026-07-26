#pragma once

#include "util.h"
#include <iostream>

class AppAttribs {
public:

	AppAttribs(int width, int height) : m_FrameBuffWidth(width), m_FrameBuffHeight(height),
		m_MenuMode(false), m_FirstMouse(false), m_EscapePressed(false), m_PrevMouseX(400), m_PrevMouseY(300),
		m_DeltaTime(0)
	{
	}

	static AppAttribs& GetAppAttribs() {
		static AppAttribs aa(WIDTH, HEIGHT);
		return aa;
	}

	inline void SetFrameBuffSizes(int width, int height) {
		m_FrameBuffWidth = width;
		m_FrameBuffHeight = height;
	}

	inline void SetEscaped(bool escaped) {
		m_EscapePressed = escaped;
	}

	inline void SetMenuMode(bool menuMode) {
		m_MenuMode = menuMode;
	}

	inline void SetFirstMouse(bool firstMouse) {
		m_FirstMouse = firstMouse;
	}

	inline void SetPrevMousePos(double x, double y) {
		m_PrevMouseX = x;
		m_PrevMouseY = y;
	}

	inline double GetPrevMouseX() {
		return m_PrevMouseX;
	}

	inline double GetPrevMouseY() {
		return m_PrevMouseY;
	}

	inline void InvertMenuModeStatus() {
		m_MenuMode = !m_MenuMode;
	}

	inline void SetDeltaTime(double dt) {
		m_DeltaTime = dt;
	}

	inline double GetDeltaTime() {
		return m_DeltaTime;
	}


	inline int GetWidth() { return m_FrameBuffWidth; }
	inline int GetHeight() { return m_FrameBuffHeight; }
	inline bool GetEscaped() { return m_EscapePressed; }
	inline bool GetMenuMode() { return m_MenuMode; }
	inline bool GetFirstMouse() { return m_FirstMouse; }



private:

	double m_DeltaTime;

	double m_PrevMouseX;
	double m_PrevMouseY;

	int m_FrameBuffWidth, m_FrameBuffHeight;

	bool m_EscapePressed;
	bool m_MenuMode;
	bool m_FirstMouse;

};
