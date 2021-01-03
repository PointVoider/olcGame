#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <fstream>
using namespace std;

int cellSize = 3;

struct sCell
{
	olc::vi2d vPos;
	olc::vi2d vSize;
	olc::vi2d vPad;
	olc::vi2d vGrid;
	olc::Pixel color = olc::BLUE;
	bool bState = false;
	int nStateCount = 0;
	int nUniqey = 69420;
	int nIdx = 0;
	bool inside(const olc::vi2d& vMousePos)
	{
		return (vMousePos.x >= vPos.x &&
				vMousePos.x <= (vPos.x + vSize.x) &&
				vMousePos.y >= vPos.y &&
				vMousePos.y <= (vPos.y + vSize.y));
	}

	bool changeState()
	{
		bState = !bState;
		nStateCount++;

		return bState;
	}


	void draw(olc::PixelGameEngine& pge)
	{
		if (bState)
			color = olc::BLUE;
		else
			color = olc::VERY_DARK_RED;
		pge.FillRect(vPos, vSize, color);
		pge.DrawRect(vPos, vSize, color.inv());
	}
};

class olcGame : public olc::PixelGameEngine
{
public:
	olcGame()
	{
		sAppName = "my olcGame";
	}
public:

	enum MBtn { LMB = 69, RMB, MMB };
	struct sHint
	{
		MBtn btn;
		vector<int> steps;
	};

public:
	vector<sCell> cells;
	int nTotalCell;
	int nCellOn;
	int nScore;
	float fDelay;
	bool bPaused = false;
	fstream file;

	vector<sHint> hint;
public:
	void ReSetCells(int& nSize)
	{
		if (nSize <= 3)nSize = 3;
		if (nSize >= 50) nSize = 50;

		if (!cells.empty())	cells.clear();
		cells.reserve(nSize * nSize);

		nCellOn = nScore = 0;
		nTotalCell = nSize * nSize;
		fDelay = 3.f;
		int a = 1;
		int index = 0;
		for (int col = 1; col <= nSize; col++) {
			for (int row = 1; row <= nSize; row++) {
				sCell c;
				c.nIdx = index;

				c.vSize = { ScreenWidth() / nSize / 2,ScreenWidth() / nSize / 2 };				//ukuran
				c.vPad = { 1,1 };																//offset jarak
				c.vGrid = { row * a ,col * a };

				c.vPos = { row * c.vSize.x + a + (row * c.vPad.x),
						   col * c.vSize.y + a + (col * c.vPad.y) };

				c.vPos.x -= ScreenWidth() / nSize / 2;
				c.vPos.y -= ScreenWidth() / nSize / 2;

				cells.push_back(std::move(c));
				index++;
			}

		}
	}

	bool OnUserCreate() override
	{
		ReSetCells(cellSize);
		sHint t = { LMB,{0,2,8,6,4} };
		hint.push_back(t);

		//file.open("Steps.txt", ios::app);
		return true;
	}

	/*bool setLeft(const sCell& c)
	{
		int index = c.nIdx - 1;

		if (cells.empty()) return false;
		if (index < 0) return false;

		auto& d = cells[index];
		if (d.vGrid.x == cellSize) return false;
		d.changeState();
		return true;

	}
	bool setRight(const sCell& c)
	{
		size_t index = c.nIdx + 1;

		if (cells.empty()) return false;
		if (index >= cells.size()) return false;

		auto& d = cells[index];
		if (d.vGrid.x == 1) return false;

		d.changeState();
		return true;

	}
	bool setUp(const sCell& c)
	{
		int index = c.nIdx - cellSize;

		if (cells.empty()) return false;
		if (index < 0) return false;

		auto& d = cells[index];
		if (d.vGrid.y == cellSize) return false;

		d.changeState();
		return true;

	}
	bool setDown(const sCell& c)
	{
		size_t index = c.nIdx + cellSize;

		if (cells.empty()) return false;
		if (index >= cells.size()) return false;

		auto& d = cells[index];
		if (d.vGrid.y == 1) return false;

		d.changeState();
		return true;

	}*/

	void setLRUD(const sCell& c)
	{
		if (cells.empty()) return;

		int leftSide = c.nIdx - 1;
		if (leftSide >= 0) {
			auto& d = cells[leftSide];
			if (d.vGrid.x != cellSize)
				if (d.changeState())
					nCellOn++;
				else
					nCellOn--;
		}

		size_t rightSide = c.nIdx + 1;
		if (rightSide < cells.size()) {
			auto& d = cells[rightSide];
			if (d.vGrid.x != 1)
				if (d.changeState())
					nCellOn++;
				else
					nCellOn--;

		}


		int upSide = c.nIdx - cellSize;
		if (upSide >= 0) {
			auto& d = cells[upSide];
			if (d.vGrid.y != cellSize)
				if (d.changeState())
					nCellOn++;
				else
					nCellOn--;
		}

		size_t downSide = c.nIdx + cellSize;
		if (downSide < cells.size()) {
			auto& d = cells[downSide];
			if (d.vGrid.y != 1)
				if (d.changeState())
					nCellOn++;
				else
					nCellOn--;
		}

	}
	void setLRUDdiag(const sCell& c)
	{
		if (cells.empty()) return;

		int upLeftSide = c.nIdx - cellSize - 1;
		if (upLeftSide >= 0) {
			auto& d = cells[upLeftSide];
			if (d.vGrid.x != cellSize)
				if (d.changeState())
					nCellOn++;
				else
					nCellOn--;
		}
		size_t upRightSide = c.nIdx - cellSize + 1;
		if (upRightSide < cells.size()) {
			auto& d = cells[upRightSide];
			if (d.vGrid.x != 1)
				if (d.changeState())
					nCellOn++;
				else
					nCellOn--;

		}


		int downLeftSide = c.nIdx + cellSize - 1;
		if (downLeftSide < cells.size()) {
			auto& d = cells[downLeftSide];
			if (d.vGrid.x != cellSize)
				if (d.changeState())
					nCellOn++;
				else
					nCellOn--;
		}
		size_t downRightSide = c.nIdx + cellSize + 1;
		if (downRightSide < cells.size()) {
			auto& d = cells[downRightSide];
			if (d.vGrid.x != 1)
				if (d.changeState())
					nCellOn++;
				else
					nCellOn--;
		}

	}

	int lastKey = -1;
	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::VERY_DARK_GREEN);
		srand(UINT(time(0)));

		auto& steps = hint.back().steps;

		if (!bPaused) {
			if (GetKey(olc::RIGHT).bReleased) ReSetCells(++cellSize);
			if (GetKey(olc::LEFT).bReleased) ReSetCells(--cellSize);

			if (GetKey(olc::R).bReleased) ReSetCells(cellSize);

			if (GetKey(olc::S).bReleased) {
				for (auto& c : cells) {
					bool bRand = rand() % 2;
					if (bRand)
						setLRUD(c);
				}
			}

			for (auto& c : cells) {


				if (GetMouse(0).bPressed) {
					if (c.inside(GetMousePos())) {
						if (lastKey == -1) {
							lastKey = 0;
							cout << "Size : " << cellSize << "\n[LMB] ";

						}
						if (lastKey != 0) {
							lastKey = 0;
							cout << endl;
							cout << "[LMB] ";
						}
						cout << c.nIdx << ' ';


						if (hint.back().steps.front() == c.nIdx) {
							steps.erase(steps.begin());
						}
						else {
							steps.insert(steps.begin(), c.nIdx);
						}

						setLRUD(c);
						nScore++;
						if (c.changeState())
							nCellOn++;
						else
							nCellOn--;

					}
				}

				if (GetMouse(1).bPressed) {
					if (c.inside(GetMousePos())) {
						if (lastKey == -1) {
							lastKey = 1;
							cout << "Size : " << cellSize << "\n[RMB] ";
						}
						if (lastKey != 1) {
							lastKey = 1;
							cout << endl;
							cout << "[RMB] ";
						}
						cout << c.nIdx << ' ';

						setLRUDdiag(c);

						nScore++;
						if (c.changeState())
							nCellOn++;
						else
							nCellOn--;
					}
				}

				if (GetMouse(2).bPressed) {
					if (nCellOn == (nTotalCell - 1))
						if (c.inside(GetMousePos())) {
							if (lastKey == -1) {
								lastKey = 2;
								cout << "Size : " << cellSize << "\n[MMB] ";
							}
							if (lastKey != 2) {
								lastKey = 2;
								cout << endl;
								cout << "[MMB] ";
							}
							cout << c.nIdx << ' ';
							if (c.changeState())
								nCellOn++;
							else
								nCellOn--;
						}
				}
			}
		}
		else {
			fDelay -= fElapsedTime;
			if (fDelay <= 1) {
				bPaused = false;
				ReSetCells(++cellSize);
			}
		}

		for (auto& c : cells) {
			c.draw(*this);


			if (!steps.empty()) {
				if (steps.front() == c.nIdx) {
					DrawString(c.vPos, to_string(hint.front().steps.size()));
				}
			}
		}

		if (nCellOn == nTotalCell) {
			bPaused = true;
			if (lastKey != -1) {
				lastKey = -1;
				cout << endl << "======================================================================\n";
			}
			DrawString(ScreenWidth() - 40 * 3, ScreenHeight() - 30, "WIN!", olc::GREEN, 3);
		}

		string sScore = "Total Clicks : " + to_string(nScore);
		string sTotal = to_string(nCellOn) + "/" + to_string(nTotalCell);
		DrawString(ScreenWidth() - (sTotal.length() * 10), 10, sTotal);

		static int posX = ScreenWidth() - (sScore.length() * 10);
		DrawString(posX, 20, sScore);

		return true;
	}
};

constexpr int sz = 2;

int main()
{
	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	olcGame game;
	if (game.Construct(1280 / sz, 720 / sz, sz, sz))
		game.Start();

	return 0;
}