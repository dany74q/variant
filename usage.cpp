#include "variant.h"
#include <string>

using namespace variant::detail;

int main() {
	Variant<int, double, std::string, std::wstring, char, bool> v2;
	{
		v2 = 4;
		int equalsFour = get<int>(v2);
		try { double throwsException = get<double>(v2); }
		catch (...) {}
	}
	{
		v2 = 5.0;
		double equalsFive = get<double>(v2);
		try { int throwsException = get<int>(v2); }
		catch (...) {}
	}
	{
		v2 = false;
		bool iamFalse = get<bool>(v2);
	}
	{
		v2 = std::string("I'm a string");
		std::string s = get<std::string>(v2);
	}
	{
		v2 = std::wstring(L"I'm a wstring");
		std::wstring s = get<std::wstring>(v2);
	}
	{
		v2 = 'a';
		char ch = get<char>(v2);
	}
	{
		// This won't compile and will print "Variant does not contains the given type" (float)
		//v2 = 0.f;
	}
}