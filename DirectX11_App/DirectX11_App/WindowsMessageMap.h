#pragma once
#include <unordered_map>
#include <Windows.h>

// WindowsMessageMap : 윈도우 메시지들을 {메시지 값(16진수) 키, 문자열 리터럴 값}의 맵으로 저장해 윈도우 메시지를 분석할 수 있도록 도와주는 클래스.
class WindowsMessageMap
{
public:
	WindowsMessageMap();
	std::string operator()(DWORD msg, WPARAM wp, LPARAM lp) const; // () 연산자를 오버로딩. Fuctor. 클래스 객체를 함수 처럼 사용할 수 있게 함.
private:
	std::unordered_map<DWORD, std::string> map;
};