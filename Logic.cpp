#include "Logic.h"

void Logic::Init(int stage)
{
	switch (stage) {
	case 1:
		step = 48; break;
	case 2:
		step = 62; break;
	case 3:
		step = 90; break;
	case 4:
		step = 125; break;
	case 5:
		step = 178; break;
	}
	HelpRoute.clear();
}

void Logic::Load(std::vector<int>& nametable, std::vector<int>& dirtable)
{
	int RoleCode;//棋子的代号0-9
	int DirCode;//移动方向的代号1,-1,2,-2
	Optimum.clear();
	for (int i = 0; i < step; ++i) {
		RoleCode = nametable[i] - 1;//代号1-10
		Move.role = RoleCode;//进行代号转换
		DirCode = dirtable[i];//代号0-3
		Move.dir = DirTrans[DirCode];//进行代号转换
		Optimum.push_back(Move);
	}
}

void Logic::Record(int RoleCode, int DirCode)
{

	DirCode = DirTrans[DirCode];
	Move.role = RoleCode;
	Move.dir = DirCode;
	if (!OutOfRoute) {
		//未偏移最优解路线时
		if (Move.dir == Optimum[Point].dir && Move.role == Optimum[Point].role)
			++Point;//行走在最优解路线上，计数加1
		else
			OutOfRoute = true;//否则偏移了路线
	}
	if (OutOfRoute) {
		//偏移了最优解路线
		HelpRoute.push_back(Move);
	}

	++Cnt;//总步数加1

}

int Logic::Help()
{
	move HelpMove;//帮助提示
	if (HelpRoute.empty()) {
		//如果返回最优解的路线为空
		OutOfRoute = false;//说明正在最优解路线上
		HelpMove = Optimum[Point + 1];//获取最优解路线的下一步
	}
	else {
		//提示返回最优解的路线
		auto iter = HelpRoute.end();//指向容器尾部
		HelpMove = *iter;//获取返回路线的最后一步
		//需要判断 修改
		HelpRoute.pop_back();//同时删除最后一步
	}
	UsedHelp = true;//记录使用了该功能，影响得分

	//处理转换
	HelpMove.role += 1;
	switch (HelpMove.dir)
	{
	case 1:HelpMove.dir = 1; break;
	case -1:HelpMove.dir = 2; break;
	case 2:HelpMove.dir = 3; break;
	case -2:HelpMove.dir = 4; break;
	}
	return HelpMove.role * 10 + HelpMove.dir;//返回提示的代号
}

bool Logic::Examine()
{
	if (!OutOfRoute)
		return false;
	//未偏移最优解路线，则不可能出现无用功
	else if(HelpRoute.empty())//如果HelpRoute为空
		return false;

	auto iter = HelpRoute.end();
	--iter;
	for ( ; iter != HelpRoute.begin();--iter ) {
		
		MoveCheck[(*iter).role] += (*iter).dir;//记录对应棋子的位置变化
		int Count = 0;//每一次循环，进行一次计数
		for (auto i : MoveCheck) {
			if (i == 0)//查看所有棋子的位置变化  
				++Count;//为0则代表回到先前状态，计数加1
		}
		++UselesStep;//假定无用的步数 进行计数
		if (Count == 10) {
			//计数为10，则说明出现无用功
			for (int Cnt = 0; Cnt < UselesStep; ++Cnt)
				HelpRoute.pop_back();
			//删除帮助路线中无用的步数
			return true;//满足条件 出现提示 
		}

	}//跳出循环，则说明不满足无用功条件

	//对记录的假定无用步数清0
	UselesStep = 0;
	//对记录棋子变化的数组清0
	for (auto i : MoveCheck)
		i = 0;
	return false;//
}

bool Logic::Revoke()
{
	if (UselesStep != 0) {
		Cnt -= UselesStep;//减去无用的步数
		UsedRevoke = true;//记录使用了该功能，影响得分
		return true;
	}
	return false;
}

void Logic::Value()
{
	if (Cnt > step)
		score -= 1;
	if (UsedHelp)
		score -= 2;
	if (UsedRevoke)
		score -= 3;

	UsedHelp = false;
	UsedRevoke = false;
}