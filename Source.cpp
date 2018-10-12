#include "olcPixelGameEngine.h"
#include <vector>

using std::to_string;
using std::vector;

class pgeHanjie : public olc::PixelGameEngine
{
public:
	pgeHanjie()
	{
		sAppName = "Hanjie";
	}

	~pgeHanjie()
	{
		delete pGrid;
	}

private:

	int nCornerX = 48;
	int nCornerY = 48;
	int nGridWidth = 10;
	int nGridHeight = 10;
	int nCellSize = 17;
	int nThumbCellSize = 5;
	unsigned char* pGrid = nullptr;

	vector<vector<int>> cluesCol;
	vector<vector<int>> cluesRow;

	int MouseGridX = 0;
	int MouseGridY = 0;

public:
	void LoadClues()
	{
		std::vector<int> tempNums;

		//Clues for each column
		tempNums = { 2 };
		cluesCol.push_back(tempNums);
		tempNums = { 4 };
		cluesCol.push_back(tempNums);
		tempNums = { 4 };
		cluesCol.push_back(tempNums);
		tempNums = { 8 };
		cluesCol.push_back(tempNums);
		tempNums = { 1,1 };
		cluesCol.push_back(tempNums);
		tempNums = { 1,1 };
		cluesCol.push_back(tempNums);
		tempNums = { 1,1,2 };
		cluesCol.push_back(tempNums);
		tempNums = { 1,1,4 };
		cluesCol.push_back(tempNums);
		tempNums = { 1,1,4 };
		cluesCol.push_back(tempNums);
		tempNums = { 8 };
		cluesCol.push_back(tempNums);

		//Clue for each Row
		tempNums = { 4 };
		cluesRow.push_back(tempNums);
		tempNums = { 3,1 };
		cluesRow.push_back(tempNums);
		tempNums = { 1,4 };
		cluesRow.push_back(tempNums);
		tempNums = { 3,1 };
		cluesRow.push_back(tempNums);
		tempNums = { 1,1 };
		cluesRow.push_back(tempNums);
		tempNums = { 1,3 };
		cluesRow.push_back(tempNums);
		tempNums = { 3,4 };
		cluesRow.push_back(tempNums);
		tempNums = { 4,4 };
		cluesRow.push_back(tempNums);
		tempNums = { 4,2 };
		cluesRow.push_back(tempNums);
		tempNums = { 2 };
		cluesRow.push_back(tempNums);
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
			for (int i = 0; i < cluesRow[y].size(); i++)
			{
				DrawString(i * 11 + 1, y * nCellSize + nCornerY + 5, to_string(cluesRow[y][i]), olc::BLACK);
				if (i < cluesRow[y].size() - 1)
					DrawString(i * 11 + 7, y * nCellSize + nCornerY + 5, ",", olc::BLACK);
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

	int ToScreenCoord(int gridCoord)
	{


		return 0;
	}

	int ToGridCoord(int screenCoord)
	{
		return (screenCoord - nCornerX) / nCellSize;
	}

	bool OnUserCreate() override
	{
		// Initialise variables
		pGrid = new unsigned char[nGridWidth*nGridHeight];
		memset(pGrid, '0', nGridWidth*nGridHeight);

		// Draw grid and clues
		FillRect(0, 0, ScreenWidth(), ScreenHeight());
		LoadClues();
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

		// Logic



		// Draw
		//Large version
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
	if (demo.Construct(400, 400, 1, 1))
		demo.Start();
	//if (demo.Construct(60, 30, 10, 10))
	//	demo.Start();

	return 0;
}