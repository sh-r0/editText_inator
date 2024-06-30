#include <iostream>
#include <format>
#include <istream>
#include <fstream>
#include <unordered_map>
#include "renderer.hpp"
#include "stb/stb_image.h"
#include "gapBuffers.hpp"

constexpr char cursorMarker_c = 219;
char* fileName_g = nullptr;
sGapBuffer_t* gapBuffPtr_g = nullptr;
size_t cursor_x = 0, cursor_y = 0;

std::unordered_map<char, char> map_c {
	{'`','~'}, {'1','!'}, {'2','@'}, {'3','#'}, {'4','$'}, {'5','%'}, {'6','^'}, {'7','&'}, {'8','*'}, {'9','('}, {'0',')'},
	{'-','_'}, {'=','+'}, {'[','{'}, {']','}'}, {';',':'}, {'\'','"'}, {'\\','|'}, {',','<'}, {'.','>'}, {'/','?'}
};

void writeFile(sGapBuffer_t& _buff);

inline char getShiftKey(const char _c) {
	return map_c[_c];
}

void key_callback(GLFWwindow* _window, int32_t _key, int32_t _scancode, int32_t _action, int32_t _mods) {
	sGapBuffer_t& gapBuffRef = *gapBuffPtr_g;
	auto& currBuff = gapBuffRef.buff_m[cursor_y];
	bool shiftState = _mods & 0x0001;
	bool controlState = _mods & 0x0002;
	bool capsLockState = _mods & 0x0010;

	if (_action == GLFW_PRESS) {
		if (controlState) {
			switch (_key) {
				case GLFW_KEY_S: {
					writeFile(gapBuffRef);
					return;
				} break;
			}
		}

		if (_key == GLFW_KEY_BACKSPACE) {
			if (cursor_x == 0) {
				if (cursor_y == 0) return;
				currBuff.moveGap(currBuff.buff.size() - currBuff.gapSize);
				std::string tmp = currBuff.buff.substr(0, currBuff.gapPosition);
				cursor_y--;
				gapBuffRef.gapPosition--;
				gapBuffRef.gapSize++;
				cursor_x = gapBuffRef.buff_m[cursor_y].buff.size() - gapBuffRef.buff_m[cursor_y].gapSize;
				gapBuffRef.buff_m[cursor_y].moveGap(cursor_x);
				gapBuffRef.buff_m[cursor_y].write(tmp);
				gapBuffRef.buff_m[cursor_y].moveGap(cursor_x);
			}
			else {
				if (controlState) {
					size_t i;
					for (i = cursor_x - 1; i > 0; i--) {
						if (currBuff.buff[i - 1] == ' ') {
							cursor_x = i;
							break;
						}
					}
					if (i == 0)
						cursor_x = 0;
					currBuff.gapSize += currBuff.gapPosition - cursor_x;
					currBuff.gapPosition = cursor_x;
					return;
				}
				cursor_x--;
				gapBuffRef.buff_m[cursor_y].gapPosition--;
				gapBuffRef.buff_m[cursor_y].gapSize++;
			}
		} else if (_key == GLFW_KEY_DELETE) {
			if (cursor_x == currBuff.buff.size() - currBuff.gapSize) {
				if (cursor_y == gapBuffRef.buff_m.size() - gapBuffRef.gapSize)
					return;
				std::string tmp;
				gapBuffRef.buff_m[cursor_y + 1].toText(tmp);
				gapBuffRef.gapSize++;
				currBuff.write(tmp);
				currBuff.moveGap(cursor_x);
			}
			else {
				if (controlState) {
					size_t i;
					for (i = currBuff.gapPosition + currBuff.gapSize+1; i < currBuff.buff.size(); i++) {
						if (currBuff.buff[i] == ' ') {
							break;
						}
					}
					currBuff.gapSize += i - currBuff.gapPosition - currBuff.gapSize;
					return;
				}
				currBuff.gapSize++;
			}
		} else if (_key == GLFW_KEY_ENTER) {
			//TODO: if(gapBuffRef.gapSize == 0)
			auto& currBuf = gapBuffRef.buff_m[cursor_y];
			currBuf.moveGap(cursor_x);
			std::string tmp = currBuf.buff.substr(currBuf.gapPosition + currBuf.gapSize, currBuf.buff.size() - cursor_x - currBuf.gapSize);
			currBuf.gapSize = currBuf.buff.size() - cursor_x;
			gapBuffRef.gapPosition++;
			gapBuffRef.gapSize--;
			cursor_y++;
			cursor_x = 0;
			gapBuffRef.buff_m[cursor_y] = gapBuffer_t();
			gapBuffRef.buff_m[cursor_y].write(tmp);
			gapBuffRef.buff_m[cursor_y].moveGap(cursor_x);
			return;
		} else if (_key >= GLFW_KEY_RIGHT && _key <= GLFW_KEY_UP) {
			switch (_key) {
				case GLFW_KEY_LEFT:
					if (cursor_x != 0) {
						if (controlState) {
							for (size_t i = cursor_x - 1; i > 0; i--) {
								if (currBuff.buff[i-1] == ' ') {
									cursor_x = i;
									currBuff.moveGap(cursor_x);
									return;
								}
							}
							cursor_x = 0;
							currBuff.moveGap(cursor_x);
							return;
						}
						cursor_x--;
						currBuff.moveGap(cursor_x);
					} else if (cursor_y != 0) {
						cursor_y--;
						cursor_x = gapBuffRef.buff_m[cursor_y].buff.size() - gapBuffRef.buff_m[cursor_y].gapSize;
						gapBuffRef.buff_m[cursor_y].moveGap(cursor_x);
					}
					break;
				case GLFW_KEY_RIGHT:
					if (cursor_x < currBuff.buff.size() - currBuff.gapSize) {
						if (controlState) {
							for (size_t i = currBuff.gapPosition + currBuff.gapSize + 1; i < currBuff.buff.size(); i++) {
								if (currBuff.buff[i] == ' ') {
									cursor_x = i - currBuff.gapSize;
									currBuff.moveGap(cursor_x);
									return;
								}
							}
							cursor_x = currBuff.buff.size() - currBuff.gapSize;
							currBuff.moveGap(cursor_x);
							return;
						}
						cursor_x++;
						currBuff.moveGap(cursor_x);
					} else if (cursor_y < gapBuffRef.buff_m.size() - gapBuffRef.gapSize -1) {
						cursor_x = 0;
						cursor_y++;
						gapBuffRef.moveGap(cursor_y+1);
						gapBuffRef.buff_m[cursor_y].moveGap(cursor_x);
					}
					break;
				case GLFW_KEY_UP:
					if (cursor_y > 0) {
						cursor_y--;
						gapBuffRef.moveGap(cursor_y+1);
						if (cursor_x >= gapBuffRef.buff_m[cursor_y].buff.size() - gapBuffRef.buff_m[cursor_y].gapSize)
							cursor_x = gapBuffRef.buff_m[cursor_y].buff.size() - gapBuffRef.buff_m[cursor_y].gapSize;
						gapBuffRef.buff_m[cursor_y].moveGap(cursor_x);
					}
					break;
				case GLFW_KEY_DOWN:
					if (cursor_y < gapBuffRef.buff_m.size() - gapBuffRef.gapSize-1) {
						cursor_y++;
						gapBuffRef.moveGap(cursor_y+1);
						if (cursor_x >= gapBuffRef.buff_m[cursor_y].buff.size() - gapBuffRef.buff_m[cursor_y].gapSize)
							cursor_x = gapBuffRef.buff_m[cursor_y].buff.size() - gapBuffRef.buff_m[cursor_y].gapSize;
						gapBuffRef.buff_m[cursor_y].moveGap(cursor_x);
					}
					break;
			}
		}
		else if (_key >= GLFW_KEY_SPACE && _key <= GLFW_KEY_GRAVE_ACCENT) {
			char charToPush = std::tolower(_key);
			if (_key >= GLFW_KEY_A && _key <= GLFW_KEY_Z) {
				if (shiftState || capsLockState) 
					charToPush -= 'a' - 'A';
			} else if (shiftState)
				charToPush = getShiftKey(charToPush);

			gapBuffRef.buff_m[cursor_y].moveGap(cursor_x);
			gapBuffRef.buff_m[cursor_y].write(charToPush);
			cursor_x++;
			gapBuffRef.buff_m[cursor_y].moveGap(cursor_x);
		}
	}
	return;
}

void loadFile(const char* _path, sGapBuffer_t& _buff) {
	std::string buff;
	std::fstream inputFile(_path, std::ios::in);
	do {
		std::getline(inputFile, buff);
		_buff.buff_m[_buff.gapPosition - 1].write(buff);
		_buff.gapPosition++;
		_buff.gapSize--;
	} while (!inputFile.eof());
	_buff.moveGap(1);
	_buff.buff_m[0].moveGap(0);
	return;
}

//TODO: optimize this shit
void writeFile(sGapBuffer_t& _buff) {
	std::fstream outStream(fileName_g, std::ios::out);
	std::string lineBuff;
	for (size_t i = 0; i < _buff.gapPosition; i++) {
		_buff.buff_m[i].toText(lineBuff);
		outStream << lineBuff + "\n";
	}
	for (size_t i = _buff.gapPosition+ _buff.gapSize; i < _buff.buff_m.size(); i++) {
		_buff.buff_m[i].toText(lineBuff);
		outStream << lineBuff + "\n";
	}
	return;
}

int32_t main(int32_t _argc, char** _argv) {
	if (_argc != 2) {
		printf("Need path to file!\n");
		return EXIT_FAILURE;
	}

	renderer_t r;
	r.initRenderer();
	sGapBuffer_t gapBuff = sGapBuffer_t();
	gapBuffPtr_g = &gapBuff;
	fileName_g = _argv[1];
	loadFile(_argv[1], gapBuff);
	for (size_t i = 0; i < gapBuff.gapPosition; i++) {
		std::cout << gapBuff.buff_m[i].buff.substr(0, gapBuff.buff_m[i].gapPosition) + "\n";
	}

	glfwSetInputMode(r.window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
	glfwSetKeyCallback(r.window, key_callback);
	
	std::string cursorChar;
	cursorChar.push_back(cursorMarker_c);
	while (!glfwWindowShouldClose(r.window)) {
		r.clearBuffers();
		size_t rLines = 0;
		for (int32_t i = -10; i < 20; i++) {
			if (int64_t(cursor_y)+i < 0) {
				continue;
			}
			if (int64_t(cursor_y) + i >= gapBuff.buff_m.size() - gapBuff.gapSize) {
				break;
			}
			gapBuffer_t& currBuff = gapBuff.buff_m[cursor_y+i];
			if (cursor_y + i >= gapBuff.gapPosition) {
				size_t diff = cursor_y + i - gapBuff.gapPosition;
				currBuff = gapBuff.buff_m[gapBuff.gapPosition + gapBuff.gapSize + diff];
			}
			if (currBuff.buff.size() != 0) {
				if (i == 0) {
					r.pushText( { 0,41+41*rLines+ -10 }, 
						currBuff.buff.substr(0, currBuff.gapPosition) + cursorChar
						+ currBuff.buff.substr(currBuff.gapPosition + currBuff.gapSize, currBuff.buff.size() - (currBuff.gapPosition + currBuff.gapSize))
					);
				}
				else {
					r.pushText({ 0,41 + 41 * rLines + -10 },
						currBuff.buff.substr(0, currBuff.gapPosition)
						+ currBuff.buff.substr(currBuff.gapPosition + currBuff.gapSize, currBuff.buff.size() - (currBuff.gapPosition + currBuff.gapSize))
					);
				}
			}
			rLines++;
		}
		r.drawFrame();
		glfwPollEvents();
	}

	writeFile(gapBuff);
	return 0;
}