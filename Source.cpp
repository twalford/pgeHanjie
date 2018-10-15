#include "olcPixelGameEngine.h"
#include <vector>
#include <istream>
#include <fstream>

using std::to_string;
using std::vector;
using std::cout;
using std::endl;

class pgeHanjie : public olc::PixelGameEngine
{
public:
	pgeHanjie()
	{
		sAppName = "Hanjie";
	}

	~pgeHanjie()
	{
		delete[] pGrid;
	}

private:

	int nCornerX = 48;
	int nCornerY = 48;
	int nGridWidth;
	int nGridHeight;
	int nCellSize = 17;
	int nThumbCellSize = 5;
	unsigned char* pGrid = nullptr;
	std::string puzzleName = "puppy";

	vector<vector<int>> cluesCol;
	vector<vector<int>> cluesRow;

	int MouseGridX = 0;
	int MouseGridY = 0;

public:

	void LoadCluesFromFile()
	{
		std::ifstream file(puzzleName + ".txt");

		if (file.is_open())
		{
			enum MODE {ROW, COL};
			MODE clueMode = COL;
			std::string line;
			std::vector<int> tempNums;

			cluesCol.clear();
			cluesRow.clear();

			while (getline(file, line))
			{
				if (line == "<cols>")
					clueMode = COL;
				else if (line == "<rows>")
					clueMode = ROW;
				else
				{
					int n = -1;
					for (auto c : line)
					{
						if (c != ',' && n < 0)
						{
							n = c - '0';
						}
						else if (c != ',' && n >= 0)
						{
							n *= 10;
							n += c - '0';
						}
						else if (c == ',')
						{
							tempNums.push_back(n);
							n = -1;
						}
						else
							std::cout << "SOMETHING WENT WRONG (CLUEMODE)" << std::endl;
					}
					tempNums.push_back(n);

					if (clueMode == COL)
						cluesCol.push_back(tempNums);
					else
						cluesRow.push_back(tempNums);

					tempNums.clear();
				}
			}

			nGridWidth = cluesCol.size();
			nGridHeight = cluesRow.size();
		}
		else
			std::cout << "UNABLE TO OPEN FILE" << std::endl;

		file.close();
	}
	void DrawClues()
	{
		//Top
		for (int x = 0; x < cluesCol.size(); x++)
			for (int i = 0; i < cluesCol[x].size(); i++)
			{
				if (cluesCol[x][i] > 9)
					DrawString(x * nCellSize + nCornerX + 1, i * 10 + 1, to_string(cluesCol[x][i]), olc::BLACK);
				else
					DrawString(x * nCellSize + nCornerX + 5, i * 10 + 1, to_string(cluesCol[x][i]), olc::BLACK);
			}

		//Side
		for (int y = 0; y < cluesRow.size(); y++)
		{
			int extraDigits = 0;

			for (int i = 0; i < cluesRow[y].size(); i++)
			{
				DrawString(i * 11 + 1 + (extraDigits*8), y * nCellSize + nCornerY + 5, to_string(cluesRow[y][i]), olc::BLACK);
				if (cluesRow[y][i] >= 10)
					extraDigits++;
				if (i < cluesRow[y].size() - 1)
					DrawString(i * 11 + 7 + (extraDigits * 8), y * nCellSize + nCornerY + 5, ",", olc::BLACK);
			}
		}
	}
	void DrawGrid()
	{
		//Draw horizontal lines in grey
		for (int i = 0; i <= nGridWidth; i++)
			DrawLine(nCornerX, nCornerY + nCellSize * i, nCornerX + nCellSize * nGridWidth, nCornerY + nCellSize * i, olc::DARK_GREY);

		//Draw vertical lines
		for (int j = 0; j <= nGridHeight; j++)
			DrawLine(nCornerX + nCellSize * j, nCornerY + 1, nCornerX + nCellSize * j, nCornerY + nCellSize * nGridHeight - 1, j % 5 == 0 ? olc::BLACK : olc::DARK_GREY);

		//Draw black horizontal lines
		for (int i = 0; i <= nGridWidth; i++)
			if (i % 5 == 0)
				DrawLine(nCornerX, nCornerY + nCellSize * i, nCornerX + nCellSize * nGridWidth, nCornerY + nCellSize * i, olc::BLACK);
	}

	int ToGridCoord(int screenCoord)
	{
		return (screenCoord - nCornerX) / nCellSize;
	}
	int TotalFromClue(std::vector<int> &clues)
	{
		int total = 0;
		for (auto c : clues)
			total += (c + 1);
		
		return total - 1;
	}
	int TotalDotsInLine(int index, char set)
	{
		int total = 0;

		if (set == 'c')
			for (int i = 0; i < nGridHeight; i++)
				if (pGrid[i*nGridWidth+index] == '2')
					total++;

		return total;
	}

	bool SolveLines(vector<vector<int>> &cluesVec, char clueOrientation)
	{
		int lineLength = clueOrientation == 'c' ? nGridHeight : nGridWidth;
		int nNumLines = clueOrientation == 'c' ? nGridWidth : nGridHeight;
		
		cout << "===== Solving " << clueOrientation << " =====" << endl;
		for (int i = 0; i < nNumLines; i++)
		{
			//create a temp line for easier data handling
			unsigned char* pLine = new unsigned char[lineLength];

			if (clueOrientation == 'c')
				for (int j = 0; j < lineLength; j++)
					pLine[j] = pGrid[j * nGridWidth + i];
			else
				for (int j = 0; j < lineLength; j++)
					pLine[j] = pGrid[i * nGridWidth + j];

			//A series of clue checks
			if (cluesVec[i][0] == lineLength)
			{
				//single clue fills whole line

				for (int j = 0; j < lineLength; j++)
					pLine[j] = '1';
			}
			else if (TotalFromClue(cluesVec[i]) == lineLength)
			{
				//multi clue fills whole line

				int ci = 0;		//clue index
				int s = 0;		//start of block
				for (int j = 0; j < lineLength; j++)
				{
					if (j < cluesVec[i][ci] + s)
						pLine[j] = '1';
					else
					{
						pLine[j] = '2';
						s = j + 1;
						ci++;
					}
				}
			}
			else
			{
				//Check how many solid in line
				int numSolid = 0;
				for (int j = 0; j < lineLength; j++)
				{
					if (pLine[j] == '1')
						numSolid++;
				}

				if (cluesVec[i].size() == 1 && cluesVec[i][0] == numSolid)
				{
					//line has the right number of blocks placed. fill rest with markers.

					for (int j = 0; j < lineLength; j++)
					{
						if (pLine[j] != '1')
							pLine[j] = '2';
					}
				}
				else if (cluesVec[i].size() == 1 && numSolid > 0)
				{
					//line has single clue and some blocks solid. attempt to place markers.

					//find first and last solid.
					int firstSolid = -1;
					int lastSolid = -1;

					for (int j = 0; j < lineLength; j++)
					{
						if (pLine[j] == '1')
						{
							//Find first and last solid blocks
							if (firstSolid == -1)
								firstSolid = j;
							lastSolid = j;
						}
					}

					//place markers where block cant extend to
					for (int j = 0; j < lineLength; j++)
					{
						if (j < lastSolid - cluesVec[i][0] || j > firstSolid + cluesVec[i][0])
							pLine[j] = '2';
					}

				}
				
				if (cluesVec[i].size() == 1 && numSolid < cluesVec[i][0])
				{
					//TODO what if there's two or more gaps. Make a struct for gaps?

					//finds the first empty gap in a line
					int gapStart = 0;
					int gapEnd = lineLength;
					int gapSize = 0;
					bool inGap = false;
					bool hasGap = false;

					for (int j = 0; j < lineLength; j++)
					{
						if (pLine[j] != '2')
						{
							if (!inGap)
							{
								gapStart = j;
								hasGap = true;
							}
							inGap = true;
						}
						else
						{
							if (inGap)
								gapEnd = j;
							inGap = false;
						}
					}

					gapSize = gapEnd - gapStart;

					//single clue fits gap
					if (cluesVec[i][0] * 2 > gapSize)
					{
						if (cluesVec[i][0] > gapSize || !hasGap)
						{
							if (!hasGap)
								cout << "i= " << i << " ERROR no gap" << endl;
							else
								cout << "i= " << i << " ERROR small gap" << endl;
							delete[] pLine;
							continue; //goto next line
						}

						int n = cluesVec[i][0] * 2 - gapSize;	//number of cells to add
						int s = (gapSize / 2) - (n / 2);		//starting pos in gap

						cout << "i= " << i << "  gs= " << gapSize << "  l= " << n << "  s= " << s << endl;

						for (int j = 0; j < n; j++)
							pLine[s + gapStart + j] = '1';
					}
					else
					{
						delete[] pLine;
						continue; //goto next line
					}
				}

			}

			//save the temp line to grid
			if (clueOrientation == 'c')
				for (int j = 0; j < lineLength; j++)
					pGrid[j * nGridWidth + i] = pLine[j];
			else
				for (int j = 0; j < lineLength; j++)
					pGrid[i * nGridWidth + j] = pLine[j];

			delete pLine;
			//end of a line

		}
		cout << "=====================" << endl;
		return true;
	}

	bool OnUserCreate() override
	{
		//Load game info
		LoadCluesFromFile();

		// Initialise grid
		pGrid = new unsigned char[nGridWidth*nGridHeight];
		memset(pGrid, '0', nGridWidth*nGridHeight);

		// Draw grid and clues
		FillRect(0, 0, ScreenWidth(), ScreenHeight());
		DrawClues();
		DrawGrid();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Input
		MouseGridX = ToGridCoord(GetMouseX());
		MouseGridY = ToGridCoord(GetMouseY());

		if (GetMouse(0).bPressed)
		{
			if (MouseGridX >= 0 && MouseGridX < nGridWidth)
			{
				if (pGrid[MouseGridY*nGridWidth + MouseGridX] == '0')
					pGrid[MouseGridY*nGridWidth + MouseGridX] = '1';
				else if (pGrid[MouseGridY*nGridWidth + MouseGridX] == '1')
					pGrid[MouseGridY*nGridWidth + MouseGridX] = '0';
				else if (pGrid[MouseGridY*nGridWidth + MouseGridX] == '2')
					pGrid[MouseGridY*nGridWidth + MouseGridX] = '1';
			}
		}

		if (GetMouse(1).bPressed)
		{
			if (MouseGridX >= 0 && MouseGridX < nGridWidth)
			{
				if (pGrid[MouseGridY*nGridWidth + MouseGridX] == '0')
					pGrid[MouseGridY*nGridWidth + MouseGridX] = '2';
				else if (pGrid[MouseGridY*nGridWidth + MouseGridX] == '1')
					pGrid[MouseGridY*nGridWidth + MouseGridX] = '2';
				else if (pGrid[MouseGridY*nGridWidth + MouseGridX] == '2')
					pGrid[MouseGridY*nGridWidth + MouseGridX] = '0';
			}
		}

		if (GetKey(olc::C).bPressed)
			SolveLines(cluesCol, 'c');
		
		if (GetKey(olc::X).bPressed)
			SolveLines(cluesRow, 'r');

		if (GetKey(olc::R).bPressed)
			memset(pGrid, '0', nGridWidth*nGridHeight);

		// Logic 
		// no logic only spaghetti


		// Draw
		for (int i = 0; i < nGridWidth; i++)
		{
			for (int j = 0; j < nGridHeight; j++)
			{
				FillRect(i * nCellSize + nCornerX + 2, j * nCellSize + nCornerY + 2, nCellSize - 3, nCellSize - 3, olc::WHITE);
				FillRect((i * nThumbCellSize) + (nGridWidth * nCellSize) + nCornerX + 32, j * nThumbCellSize + nCornerY + 1, nThumbCellSize, nThumbCellSize, olc::WHITE);

				switch (pGrid[j*nGridWidth + i])
				{
				case '1':
					//black square
					FillRect(i * nCellSize + nCornerX + 2, j * nCellSize + nCornerY + 2, nCellSize - 3, nCellSize - 3, olc::BLACK);
					FillRect((i * nThumbCellSize) + (nGridWidth * nCellSize) + nCornerX + 32, j * nThumbCellSize + nCornerY + 1, nThumbCellSize, nThumbCellSize, olc::BLACK);
					break;
				case '2':
					//grey dot
					FillRect(i * nCellSize + nCornerX + 8, j * nCellSize + nCornerY + 8, 2, 2, olc::DARK_GREY);
					break;
				default:
					break;
				}
			}
		}


		// Debug display
		//FillRect(0, 300, 100, 16);
		//DrawString(0, 300, "x = " + to_string(MouseGridX), olc::BLACK);
		//DrawString(0, 308, "y = " + to_string(MouseGridY), olc::BLACK);

		return true;
	}


};

int main()
{
	pgeHanjie demo;
	if (demo.Construct(800, 600, 1, 1))
		demo.Start();
	
	return 0;
}