#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <fstream>

#pragma warning(disable:4018)

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

	bool Inside(const olc::vi2d& vMousePos)
	{
		return (vMousePos.x >= vPos.x &&
				vMousePos.x <= (vPos.x + vSize.x) &&
				vMousePos.y >= vPos.y &&
				vMousePos.y <= (vPos.y + vSize.y));
	}

	bool Toggle()
	{
		bState = !bState;
		nStateCount++;

		return bState;
	}


	void Draw(olc::PixelGameEngine* pge)
	{
		if (bState)
			color = olc::Pixel(40, 112, 202);
		else
			color = olc::Pixel(123, 55, 52);
		pge->FillRect(vPos, vSize, color);
		pge->DrawRect(vPos, vSize, color.inv());
	}


	sCell& getTopCell()
	{
		sCell null;
		sCell* c = this;
		c -= cellSize;
		if (c->nUniqey != 69420) return null;
		return *c;
	}
	sCell& getBottomCell()
	{
		sCell null;
		sCell* c = this;
		c += cellSize;
		if (c->nUniqey != 69420) return null;
		return *c;
	}
	sCell& getLeftCell()
	{
		sCell null;
		sCell* c = this;
		c -= 1;
		if (c->nUniqey != 69420) return null;
		if (c->vGrid.x == cellSize) return null;
		return *c;
	}
	sCell& getRightCell()
	{
		sCell null;
		sCell* c = this;
		c += 1;
		if (c->nUniqey != 69420) return null;
		if (c->vGrid.x == 1) return null;

		return *c;
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

	enum MBtn { LMB = -69, RMB, MMB };
public:
	vector<sCell> cells;
	int nTotalCell;
	int nCellOn;
	int nScore;
	int nStep;
	float fDelay;
	bool bPaused = false;
	//fstream file;

	vector<pair<MBtn, int>> hint;
public:
	void ReSetCells(int& nSize)
	{
		if (nSize <= 3)nSize = 3;
		if (nSize >= 50) nSize = 50;

		if (!cells.empty())	cells.clear();
		cells.reserve(nSize * nSize);

		switch (nSize) {
		case 3:
			hint = loadHint({ 1,1,1,1,1,LMB,0,2,8,6,4 });
			break;
		case 4:
			hint = loadHint({ LMB,0,3,9,10,13,14 });
			break;
		case 5:
			hint = loadHint({ LMB,0,4,7,20,24,17,11,13,RMB,12 });
			break;
		case 6:
			hint = loadHint({ LMB,0,5,8,9,14,15,18,23,31,34,32,33,
							RMB,31,LMB,33,32,RMB,32,LMB,31,30,MMB,31 });
			break;
		case 7:
			hint = loadHint({ LMB,0,9,6,11,10,
							RMB,15,15,
							LMB,16,18,21,27,
							RMB,24,
							LMB,36,40,38,
							RMB,45,
							LMB,47,46,
							RMB,46,
							LMB,45,
							RMB,45,
							LMB,45,44,
							RMB,44,
							LMB,43 });
			break;
		case 8:
			hint = loadHint({ LMB,0,7,10,13,12,11,18,18,18,20,20,21,
							RMB,27,28,
							LMB,24,31,46,41,51,52,63,56,59,60 });
			break;
		case 9:
			hint = loadHint({ LMB,0,11,4,15,8,80,69,76,65,72,27,45,
							RMB,29,38,38,47,
							LMB,37,
							RMB,38,
							LMB,35,53,
							RMB,33,51,
							LMB,43,
							RMB,42,
							LMB,39,31,41,49,
							MMB,40 });
			break;
		case 10:
			hint = loadHint({ LMB,0,90,99,9,12,17,
							RMB,14,15,
							LMB,82,87,
							RMB,84,85,
							LMB,30,60,
							RMB,32,62,
							LMB,43,53,39,69,
							RMB,37,67,
							LMB,46,56,
							RMB,35,34,
							LMB,34,35,
							RMB,64,65,
							LMB,64,65,
							RMB,44,45,64,64,54,55,
							LMB,44,45,54,55,53,54,55,56,
							RMB,66,
							LMB,66,
							RMB,63,
							LMB,63,64,65,
							RMB,76,73,
							LMB,74,75,86,
							RMB,86,
							LMB,83,
							RMB,83,
							LMB,85,84,95,94,
							RMB,97,92,
							LMB,91,98,96,96,98,97,97,
							RMB,97,
							LMB,96,95,
							RMB,94,92,
							LMB,91,
							MMB,92 });
			break;
		case 11:
			hint = loadHint({});
			break;
		default:
			hint = loadHint({});
			break;
		}


		nStep = hint.size();
		nCellOn = nScore = 0;
		nTotalCell = nSize * nSize;
		fDelay = 3.f;	//3 detik
		int a = 1;
		int index = 0;
		for (int col = 1; col <= nSize; col++) {
			for (int row = 1; row <= nSize; row++) {
				sCell c;
				c.nIdx = index;

				c.vSize = { ScreenHeight() / nSize - 2, ScreenHeight() / nSize - 2 };		//ukuran
				c.vPad = { 1,1 };															//offset jarak
				c.vGrid = { row * a , col * a };

				c.vPos = { row * c.vSize.x + a + (row * c.vPad.x) - c.vSize.x,
						   col * c.vSize.y + a + (col * c.vPad.y) - c.vSize.y };

				cells.push_back(std::move(c));
				index++;
			}

		}
	}


	vector<pair<MBtn, int>> loadHint(vector<int> hints)
	{
		vector<pair<MBtn, int>> res;
		MBtn b[3] = { LMB,RMB,MMB };
		MBtn btn;

		bool btnInit = false;
		bool skip = false;

		for (int i = 0; i < hints.size(); i++) {
			skip = false;
			for (int j = 0; j < 3; j++) {
				if (hints.at(i) == b[j]) {
					btn = b[j];
					skip = true;
					if (!btnInit)
						btnInit = true;
					break;
				}
			}
			if (!btnInit)	continue;
			if (skip)		continue;


			res.push_back({ btn,hints.at(i) });

		}
		return res;
	}

	bool OnUserCreate() override
	{
		ReSetCells(cellSize);
		return true;
	}

#pragma region Get Cells
	sCell& getTopCell(const sCell& c, int offset = 1)
	{
		int idx = c.nIdx - (cellSize * offset);
		sCell tmp;// (idx < 0) ? cells[0] : cells[idx];
		return (idx < 0) ? tmp : cells[idx];
	}
	sCell& getBottomCell(const sCell& c, int offset = 1)
	{
		int idx = c.nIdx + (cellSize * offset);
		sCell tmp;
		return (idx >= cells.size()) ? tmp : cells[idx];
	}
	sCell& getLeftCell(const sCell& c, int offset = 1)
	{
		int idx = c.nIdx - offset;
		sCell tmp;
		auto& cell = (idx < 0 || offset >= cellSize) ? tmp : cells[idx];

		while (offset > 0 && idx >= 0 && offset < cellSize) {
			if (cells[idx++].vGrid.x == cellSize) return tmp;
			offset--;
		}
		return cell;
	}
	sCell& getRightCell(const sCell& c, int offset = 1)
	{
		int idx = c.nIdx + offset;
		sCell tmp;
		auto& cell = (idx >= cells.size() || offset >= cellSize) ? tmp : cells[idx];

		while (offset > 0 && idx < cells.size() && offset < cellSize) {
			if (cells[idx--].vGrid.x == 1) return tmp;
			offset--;
		}
		return cell;
	}
#pragma endregion


	void setLRUD(const sCell& c)
	{
		if (cells.empty()) return;

		int leftSide = c.nIdx - 1;
		if (leftSide >= 0) {
			auto& d = cells[leftSide];
			if (d.vGrid.x != cellSize)
				d.Toggle();
		}

		size_t rightSide = c.nIdx + 1;
		if (rightSide < cells.size()) {
			auto& d = cells[rightSide];
			if (d.vGrid.x != 1)
				d.Toggle();

		}


		int upSide = c.nIdx - cellSize;
		if (upSide >= 0) {
			auto& d = cells[upSide];
			if (d.vGrid.y != cellSize)
				d.Toggle();
		}

		size_t downSide = c.nIdx + cellSize;
		if (downSide < cells.size()) {
			auto& d = cells[downSide];
			if (d.vGrid.y != 1)
				d.Toggle();
		}

	}
	void setLRUDdiag(const sCell& c)
	{
		if (cells.empty()) return;

		int upLeftSide = c.nIdx - cellSize - 1;
		if (upLeftSide >= 0) {
			auto& d = cells[upLeftSide];
			if (d.vGrid.x != cellSize)
				d.Toggle();
		}
		size_t upRightSide = c.nIdx - cellSize + 1;
		if (upRightSide < cells.size()) {
			auto& d = cells[upRightSide];
			if (d.vGrid.x != 1)
				d.Toggle();

		}


		int downLeftSide = c.nIdx + cellSize - 1;
		if (downLeftSide < cells.size()) {
			auto& d = cells[downLeftSide];
			if (d.vGrid.x != cellSize)
				d.Toggle();
		}
		size_t downRightSide = c.nIdx + cellSize + 1;
		if (downRightSide < cells.size()) {
			auto& d = cells[downRightSide];
			if (d.vGrid.x != 1)
				d.Toggle();
		}

	}

	void setTBLR(sCell& c, int size)
	{
		for (int i = 1; i <= size; i++) {
			getTopCell(c, i).Toggle();
			getBottomCell(c, i).Toggle();
			getLeftCell(c, i).Toggle();
			getRightCell(c, i).Toggle();
		}
	}

	int lastBtn = -1;
	bool bLogBtn = false;
	bool bHints = false;
	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::Pixel(46, 50, 58));
		srand(UINT(time(0)));

		if (!bPaused) {

			if (GetKey(olc::RIGHT).bReleased) ReSetCells(++cellSize);
			if (GetKey(olc::LEFT).bReleased) ReSetCells(--cellSize);

			if (GetKey(olc::R).bReleased) ReSetCells(cellSize);

			if (GetKey(olc::H).bReleased) bHints = !bHints;

			if (GetKey(olc::S).bReleased) {
				for (auto& c : cells) {
					bool bRand = rand() % 2;
					if (bRand)
						setLRUD(c);
				}
			}

			bool pressed = false;
			for (auto& c : cells) {
				if (c.Inside(GetMousePos()) && GetKey(olc::K).bPressed)
				{
					c.getTopCell().Toggle();
					c.getBottomCell().Toggle();
					c.getLeftCell().Toggle();
					c.getRightCell().Toggle();
				}
				if (GetMouse(0).bPressed) {
					if (c.Inside(GetMousePos())) {
						//////////////////////////////////////////////
						if (bLogBtn) {
							if (lastBtn == -1) {
								lastBtn = 0;
								cout << "Size : " << cellSize << "\n[LMB] ";

							}
							if (lastBtn != 0) {
								lastBtn = 0;
								cout << endl;
								cout << "[LMB] ";
							}
							cout << c.nIdx << ' ';
						}
						///////////////////////////////////////////
						if (!hint.empty()) {
							if (nStep > 0) {

								if (hint.front().first == LMB) {
									if (hint.front().second == c.nIdx) {
										rotate(hint.begin(), hint.begin() + 1, hint.end());
										nStep--;
									}
									else {
										hint.insert(hint.begin(), { LMB,c.nIdx });
										nStep++;
									}
								}
								else {
									hint.insert(hint.begin(), { LMB,c.nIdx });
									nStep++;
								}


							}
						}
						else {
							hint.insert(hint.begin(), { LMB,c.nIdx });
							nStep++;
						}

						if (nStep < 1)
							hint.clear();
						//////////////////////////////

						setLRUD(c);
						//setTBLR(c, cellSize);
						nScore++;
						c.Toggle();

					}
				}

				if (GetMouse(1).bPressed) {
					if (c.Inside(GetMousePos())) {
						//////////////////////////////////////////////////
						if (bLogBtn) {
							if (lastBtn == -1) {
								lastBtn = 1;
								cout << "Size : " << cellSize << "\n[RMB] ";
							}
							if (lastBtn != 1) {
								lastBtn = 1;
								cout << endl;
								cout << "[RMB] ";
							}
							cout << c.nIdx << ' ';
						}
						//////////////////////////////////////////////////

						if (!hint.empty()) {
							if (nStep > 0) {

								if (hint.front().first == RMB) {
									if (hint.front().second == c.nIdx) {
										rotate(hint.begin(), hint.begin() + 1, hint.end());
										nStep--;
									}
									else {
										hint.insert(hint.begin(), { RMB,c.nIdx });
										nStep++;
									}
								}
								else {
									hint.insert(hint.begin(), { RMB,c.nIdx });
									nStep++;
								}


							}
						}
						else {
							hint.insert(hint.begin(), { RMB,c.nIdx });
							nStep++;
						}

						if (nStep < 1)
							hint.clear();
						///////////////////////////////////////////////////
						setLRUDdiag(c);

						nScore++;
						c.Toggle();
					}
				}
				if (GetKey(olc::ENTER).bPressed && bHints) {
					if (!pressed && !hint.empty() && nStep > 0) {
						if (c.nIdx == hint.front().second) {
							if (hint.front().first == RMB && !pressed) {
								setLRUDdiag(c);
								c.Toggle();
								rotate(hint.begin(), hint.begin() + 1, hint.end());
								pressed = true;
								nStep--;
							}

							if (hint.front().first == LMB && !pressed) {
								setLRUD(c);
								c.Toggle();
								rotate(hint.begin(), hint.begin() + 1, hint.end());
								pressed = true;
								nStep--;
							}

							if (hint.front().first == MMB && !pressed) {
								c.Toggle();
								pressed = true;
								nStep--;
							}
						}
					}
				}
				//if (GetMouse(2).bPressed) {
				//	if (nCellOn == (nTotalCell - 1))
				//		if (c.Inside(GetMousePos())) {
				//			if (lastBtn == -1) {
				//				lastBtn = 2;
				//				cout << "Size : " << cellSize << "\n[MMB] ";
				//			}
				//			if (lastBtn != 2) {
				//				lastBtn = 2;
				//				cout << endl;
				//				cout << "[MMB] ";
				//			}

				//			//////////////////////////////////
				//			if (!hint.empty()) {
				//				if (nStep > 0) {

				//					if (hint.front().first == MMB) {
				//						if (hint.front().second == c.nIdx) {
				//							rotate(hint.begin(), hint.begin() + 1, hint.end());
				//							nStep--;
				//						}
				//						else {
				//							hint.insert(hint.begin(), { MMB,c.nIdx });
				//							nStep++;
				//						}
				//					}
				//					else {
				//						hint.insert(hint.begin(), { MMB,c.nIdx });
				//						nStep++;
				//					}


				//				}
				//			}
				//			///////////////////////////////////


				//			cout << c.nIdx << ' ';
				//			c.Toggle();
				//		}
				//}
			}
		}
		else {
			fDelay -= fElapsedTime;
			if (nStep)nStep = 0;
			if (fDelay <= 1) {
				bPaused = false;
				ReSetCells(++cellSize);
			}
		}

		nCellOn = 0;
		for (auto& c : cells) {
			c.Draw(this);
			if (c.bState) nCellOn++;
		}

		for (auto& c : cells) {
			if (!hint.empty() && nStep > 0 && bHints) {
				if (hint.front().second == c.nIdx) {
					string sBtn;
					olc::Pixel col;
					if (hint.front().first == LMB) {
						sBtn = "L ";
						col = olc::GREEN;
					}
					else if (hint.front().first == RMB) {
						sBtn = "R ";
						col = olc::CYAN;
					}
					else if (hint.front().first == MMB) {
						sBtn = "M ";
						col = olc::GREY;
					}
					string sStep = sBtn + to_string(nStep);
					DrawString(c.vPos.x + 2, c.vPos.y + 2, sStep, col);
				}
			}
			if (GetMouse(2).bPressed) {
				if (nCellOn == (nTotalCell - 1))
					if (c.Inside(GetMousePos())) {
						if (c.bState == false)
							c.Toggle();
					}
			}
		}

		if (nCellOn == nTotalCell) {
			bPaused = true;
			if (bLogBtn)
				if (lastBtn != -1) {
					lastBtn = -1;
					cout << endl << "======================================================================\n";
				}
			DrawString(ScreenWidth() - 40 * 3, ScreenHeight() - 30, "WIN!", olc::GREEN, 3);
		}


		if (!cells.empty())
			if (bHints)
				DrawString(cells[cellSize - 1].vPos.x + cells[cellSize - 1].vSize.x + 2, cells[cellSize * cellSize - 1].vPos.y + cells[cellSize * cellSize - 1].vSize.y - 10, "Hint ON");

		string sScore = "Total Clicks : " + to_string(nScore);
		string sTotal = to_string(nCellOn) + "/" + to_string(nTotalCell);
		DrawString(ScreenWidth() - (sTotal.length() * 10), 10, sTotal);

		static olc::vi2d pos = { (int)(ScreenWidth() - (sScore.length() * 10)),20 };
		DrawString(pos, sScore);

		return true;
	}
};

constexpr int sz = 2;

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	olcGame game;
	if (game.Construct(1280 / sz, 720 / sz, sz, sz))
		game.Start();

	return 0;
}