#ifndef GAME_H_
#define GAME_H_

#include "commander.h"
#include "i_deploy_helper.h"
#include "map.h"
#include "unit.h"
#include "common.h"
#include "turn.h"
#include "resource_manager.h"
#include <string>
#include <vector>
#include <functional>

class Assets;
class LuaScript;
class Magic;
class Deployer;

class Game : public IDeployHelper {
 public:
  enum class Status {
    kNone,
    kDeploying,
    kUndecided,
    kVictory,
    kDefeat
  };

 public:
  Game(const ResourceManagers&, Assets*, const string&);
  ~Game();

 public:
  // Map related //
  Map* GetMap() { return map_; }
  Vec2D GetMapSize() { return map_->GetSize(); }
  std::string GetMapBitmapPath() { return map_->GetBitmapPath(); }

  // General //
  void ForEachUnit(std::function<void(Unit*)>);
  void MoveUnit(Unit*, Vec2D);
  void KillUnit(Unit*);
  Cmd* BasicAttackUnit(Unit*, Unit*, bool);
  bool IsValidCoords(Vec2D);
  Magic* GetMagic(const std::string&);
  Unit* GetUnit(int);
  Equipment* GetEquipment(const std::string&);
  MagicManager* GetMagicManager() { return rc_.magic_manager; }
  LuaScript* GetLuaScript() { return lua_script_; }
  bool EndSideTurn();
  bool IsCurrentTurn(Unit*) const;
  bool IsAITurn() const;
  bool IsUserTurn() const;
  uint16_t GetTurnCurrent() const;
  uint16_t GetTurnLimit() const;
  bool HasPendingCmd() const;
  void DoPendingCmd();
  void PushCmd(unique_ptr<Cmd>);
  const Cmd* GetNextCmdConst() const;
  bool UnitInCell(Vec2D) const;
  uint32_t GetNumEnemiesAlive();
  uint32_t GetNumOwnsAlive();
  bool CheckStatus();
  Status GetStatus() { return status_; }
  void Begin();

  // IDeployHelper interfaces
  void SubmitDeploy() override;
  uint32_t AssignDeploy(const shared_ptr<Hero>&) override;
  void UnassignDeploy(const shared_ptr<Hero>&) override;

  // APIs for Lua //
  void AppointHero(const string&, uint16_t);
  uint32_t GenerateOwnUnit(const string&, Vec2D);
  uint32_t GenerateOwnUnit(shared_ptr<Hero>, Vec2D);
  uint32_t GenerateUnit(const string&, uint16_t, Unit::Side, Vec2D);
  void ObtainEquipment(const string&, uint32_t);
  bool UnitPutWeaponOn(uint32_t, const string&);

  // APIs for AI //
  vector<Unit*> GetCurrentUnits();
  vector<Vec2D> FindMovablePos(Unit*);
  PathTree*     FindMovablePath(Unit*);
  Unit*         GetOneHostileInRange(Unit*, Vec2D);

 private:
  void InitLua(const string&);
  bool TryBasicAttack(Unit*, Unit*);
  bool TryMagic(Unit*, Unit*);

 private:
  ResourceManagers   rc_;
  Assets*            assets_;
  LuaScript*         lua_script_;
  Commander          commander_;
  Deployer*          deployer_;
  Map*               map_;
  std::vector<Unit*> units_;
  std::vector<Unit*> dead_units_;
  Turn               turn_;
  Status             status_;
};

#endif
