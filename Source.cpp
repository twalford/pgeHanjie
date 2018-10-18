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
	int LargestFromClue(std::vector<int> &clues)
	{
		int largest = 0;

		for (auto c : clues)
			if (c > largest)
				largest = c;

		return largest;
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
			//create temp lines
			unsigned char* pLine = new unsigned char[lineLength];
			unsigned char* pLeft = new unsigned char[lineLength];
			unsigned char* pRight = new unsigned char[lineLength];
			memset(pLeft, '0', lineLength);
			memset(pRight, '0', lineLength);

			//copy over the line data
			if (clueOrientation == 'c')
				for (int j = 0; j < lineLength; j++)
					pLine[j] = pGrid[j * nGridWidth + i];
			else
				for (int j = 0; j < lineLength; j++)
					pLine[j] = pGrid[i * nGridWidth + j];
			
			//Compact the clues to the left side, save it to pLeft
			int ci = 0;		//clue index
			int s = 0;		//start of block

			for (int j = 0; j < lineLength; j++)
			{
				if (ci >= cluesVec[i].size())
					pLeft[j] = '0';
				else
				{
					if (j < cluesVec[i][ci] + s)
						pLeft[j] = ci - '0';
					else
					{
						pLeft[j] = '0';
						s = j + 1;
						ci++;
					}
				}
					
			}

			//Compact the clues to the right side, save it to pRight
			ci = cluesVec[i].size() - 1;	//clue index
			s = lineLength;					//start of block

			for (int j = lineLength - 1; j >= 0; j--)
			{
				if (ci < 0)
					pRight[j] = '0';
				else
				{
					if (j > s - cluesVec[i][ci] - 1)
						pRight[j] = ci - '0';
					else
					{
						pRight[j] = '0';
						s = j - 1;
						ci--;
					}
				}
			}

			//Find where clues overlap in pLeft and pRight
			int gap = lineLength - TotalFromClue(cluesVec[i]);

			if (LargestFromClue(cluesVec[i]) > gap)
			{
				for (int j = 0; j < lineLength; j++)
				{
					if (pLeft[j] == pRight[j] && pRight[j] != '0')
						pLine[j] = '1';
				}
			}

			//Check how many solid in line
			int numSolid = 0;
			for (int j = 0; j < lineLength; j++)
			{
				if (pLine[j] == '1')
					numSolid++;
			}

			//if line has the right number of blocks placed. fill rest with markers.
			if (cluesVec[i].size() == 1 && cluesVec[i][0] == numSolid)
			{
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

				for (int j = 0; j < lineLength; j++)
				{
					//place markers where block cant extend to
					if (j < lastSolid - cluesVec[i][0] + 1 || j >= firstSolid + cluesVec[i][0])
						pLine[j] = '2';

					//place solid between first and last solid
					//if (j > firstSolid && j < lastSolid)
					//	pLine[j] = '1';
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
						//debug
						if (!hasGap)
							cout << "i= " << i << " ERROR no gap" << endl;
						else
							cout << "i= " << i << " ERROR small gap" << endl;
					}
					else
					{
						int n = cluesVec[i][0] * 2 - gapSize;	//number of cells to add
						int s = (gapSize / 2) - (n / 2);		//starting pos in gap

						cout << "i= " << i << "  gs= " << gapSize << "  l= " << n << "  s= " << s << endl;

						for (int j = 0; j < n; j++)
							pLine[s + gapStart + j] = '1';
					}
				}
				
			}

			
			// =============== END OF OLD ===============*/

			//save the temp line to grid
			if (clueOrientation == 'c')
				for (int j = 0; j < lineLength; j++)
					pGrid[j * nGridWidth + i] = pLine[j];
			else
				for (int j = 0; j < lineLength; j++)
					pGrid[i * nGridWidth + j] = pLine[j];

			//free the memory
			delete[] pLine;
			delete[] pLeft;
			delete[] pRight;
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
