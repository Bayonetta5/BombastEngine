#pragma once

class GameSampleApp : public BombastApp
{
protected:
	virtual CoreGameLogic *VCreateGameAndView();

public:
	virtual TCHAR *VGetGameTitle() { return _T("Bombast Engine Sample Game"); }
	//virtual TCHAR *VGetGameAppDirectory() { return _T(""); }

protected:
};


class GameSampleLogic : public CoreGameLogic
{
protected:

public:
	GameSampleLogic();
	virtual ~GameSampleLogic();

	virtual void VMoveActor(const ActorId id, Mat4x4 const &mat);

	virtual void VChangeState(CoreGameState newState);

	void RequestStartGameDelegate(IEventDataPtr pEventData);
	void StartMoveUpDelegate(IEventDataPtr pEventData);
	void EndMoveUpDelegate(IEventDataPtr pEventData);
	void StartMoveRightDelegate(IEventDataPtr pEventData);
	void EndMoveRightDelegate(IEventDataPtr pEventData);
	void StartMoveForwardDelegate(IEventDataPtr pEventData);
	void EndMoveForwardDelegate(IEventDataPtr pEventData);

	void StartTurnRightDelegate(IEventDataPtr pEventData);
	void EndTurnRightDelegate(IEventDataPtr pEventData);
	void StartTurnUpDelegate(IEventDataPtr pEventData);
	void EndTurnUpDelegate(IEventDataPtr pEventData);

protected:
	virtual bool VLoadGameDelegate(rapidxml::xml_node<>* pLevelData);

private:
	void RegisterAllDelegates();
	void RemoveAllDelegates();
};