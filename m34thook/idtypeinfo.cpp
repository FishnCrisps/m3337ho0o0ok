#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "idtypeinfo.hpp"
#include "scanner_core.hpp"
#include <string>
#include <set>
#include <vector>
static constexpr const char* g_all_entity_names[] = {
"jakesDevWidget","idWorldspawn","idWorldClipBounds","idWeaponEntity","idWaveShield","idWaterEntity","idVolume_VisualImpairment","idVolume_Trigger","idVolume_TraversalGen","idVolume_ToggleableDamageOverTime","idVolume_TogglePlayerBodyReaction","idVolume_StreamTreeMarkup","idVolume_StealthField","idVolume_StatusEffect","idVolume_Stairs","idVolume_SpaceWarp","idVolume_Siphon","idVolume_SecretHint","idVolume_RuneHint","idVolume_RunCycleHint","idVolume_RevivePlayer","idVolume_PlayerViewEffect","idVolume_PlayerUseProxy","idVolume_PlayerEnvOverride","idVolume_PlayerBodyReaction","idVolume_PlasmaPuddleFX","idVolume_PlasmaPuddle","idVolume_PlacedFlightVolume","idVolume_OliviasGuardFight_DangerZone","idVolume_MatterBallWarp","idVolume_MancubusSteamFX","idVolume_MancubusSteam","idVolume_MancubusFlamethrower","idVolume_LocationCalling","idVolume_LightRigModifier","idVolume_KillDownedInjured","idVolume_InvasionSpawnZone","idVolume_InvasionInhibit","idVolume_Invasion","idVolume_Gravity","idVolume_ForceDormant","idVolume_FlightObstacle","idVolume_Flight","idVolume_EmpField","idVolume_DoomGravity","idVolume_DeployedLaser","idVolume_DemonicPossession","idVolume_DemonicBait","idVolume_DamageOverTime","idVolume_Damage","idVolume_CustomLedgeGrab","idVolume_ControlPointSpawnInfluence","idVolume_Blocking","idVolume","idUtilityCamera","idUmbraVolume","idUmbraVisibilityContributionVolume","idTurret","idTrigger_VoiceCommunication","idTrigger_VisibilityController","idTrigger_TestPlayerState","idTrigger_Teleporter_Fade","idTrigger_Teleporter","idTrigger_TakeDamage","idTrigger_StatDrivenEvent","idTrigger_SonicBoom","idTrigger_Repulsor","idTrigger_RemoveInventoryItems","idTrigger_RemoveEntities","idTrigger_Push","idTrigger_Multiplayer","idTrigger_MovementModifier","idTrigger_ModPlayerVelocity","idTrigger_InvasionAreaExit","idTrigger_InvasionAreaEnter","idTrigger_Hurt","idTrigger_GorillaBar","idTrigger_ForcePlayerBodyReactionOnce","idTrigger_Facing","idTrigger_EnergyField","idTrigger_DynamicHurt","idTrigger_DummyFire","idTrigger_Dormancy","idTrigger_DemonDamageOutModifier","idTrigger_Damage","idTrigger_ChallengeBoundry","idTrigger_BouncePad","idTrigger_AIInteraction","idTriggerToucher","idTrigger","idTrailerCamera","idToucher","idTexlodNodeGenerationArea","idTest_Sector","idTest_MetaData_WebPath","idTest_MetaData_Target","idTest_MetaData_String","idTest_MetaData_SectorYaw","idTest_MetaData_SectorPitch","idTest_MetaData_Plane","idTest_MetaData_MoveableSpheres","idTest_MetaData_Model","idTest_MetaData_Line","idTest_MetaData_FovYaw","idTest_MetaData_FovPitch","idTest_MetaData_Fov","idTest_MetaData_Cylinder2","idTest_MetaData_Cylinder","idTest_MetaData_Circle","idTest_MetaData_Bounds","idTestModel2","idTestEntity","idTeleporterPad","idTeamCapturePoint","idTarget_VisibilityController","idTarget_ViewPos","idTarget_UnlockUltraNightmare","idTarget_TutorialGui","idTarget_Timer","idTarget_Timeline","idTarget_TestPlayerState","idTarget_Teleport","idTarget_SwapNavMesh","idTarget_SwapFaction","idTarget_Subtitle","idTarget_StartSoundShader","idTarget_Spawn_Target","idTarget_Spawn_Parent","idTarget_Spawn","idTarget_SoundDuck","idTarget_Snap_Objective","idTarget_ShowHands","idTarget_ShowGui","idTarget_Show","idTarget_ShakeTrigger","idTarget_SetSpawnSpot","idTarget_SetInEncounterGroup","idTarget_SetGroupCombatStage","idTarget_Secret","idTarget_RemoveSaveGate","idTarget_RemoveItems","idTarget_Remove","idTarget_RandomImpulse","idTarget_Print","idTarget_PodiumSpawn","idTarget_PlayerWhiplash","idTarget_PlayerViewEffect","idTarget_PlayerStatus","idTarget_PlayerStatModifier","idTarget_PlayerModifier","idTarget_PlayerInvulnerability","idTarget_PlayerCheckpoint","idTarget_PlayVoiceOver","idTarget_Ping","idTarget_Perk","idTarget_Path","idTarget_POI","idTarget_Objective_Triggered","idTarget_Objective_Replace","idTarget_Objective_HideEntities","idTarget_Objective_Give","idTarget_Objective_Complete","idTarget_Notification","idTarget_ModifyTraversalClass","idTarget_ModifyGroup","idTarget_Melee","idTarget_MapGroupUnlock","idTarget_MakeActivatable","idTarget_LightController","idTarget_LevelTransition","idTarget_LayerStateChange","idTarget_InventoryCheck","idTarget_Intro","idTarget_InteractionAction","idTarget_HideHands","idTarget_Hide","idTarget_GroupMessage","idTarget_GroupExpression","idTarget_GiveWeaponUpgradePoints","idTarget_GiveItems","idTarget_GeomCacheStreamer","idTarget_GameStateIntSet","idTarget_GameStateIntInc","idTarget_GameChallengeGameOver","idTarget_GUICommand","idTarget_ForceGroupRole","idTarget_ForceDormancy","idTarget_FirstThinkActivate","idTarget_FireWeapon","idTarget_FastTravelUnlock","idTarget_FastTravelInhibit","idTarget_FakeEnemy","idTarget_FadeComplete","idTarget_EquipItem","idTarget_Enemy","idTarget_EndOfCampaign","idTarget_EndInvasion","idTarget_EncounterChallenge","idTarget_EnableTarget","idTarget_EnableGroup","idTarget_EnableAIEvent","idTarget_DynamicChallenge_Start","idTarget_DynamicChallenge_PointTrigger","idTarget_DynamicChallenge_GiveBonus","idTarget_DynamicChallenge_FailChallenge","idTarget_DynamicChallenge_End","idTarget_DummyFire","idTarget_DormancyRadius","idTarget_Disconnect_GoToScreen","idTarget_Disconnect","idTarget_DisableInvasion","idTarget_DisableEscMenu","idTarget_DevLoadoutSwap","idTarget_DemonBountyAiTypes","idTarget_DeactivateStatusEffects","idTarget_Damage","idTarget_Cvar","idTarget_CurrencyCheck","idTarget_Credits","idTarget_Count_Random","idTarget_Count","idTarget_ConditionalAccessor","idTarget_Conditional","idTarget_Command","idTarget_CollisionDamage","idTarget_Codex","idTarget_ClearFakeEnemy","idTarget_ChangeVolume_PlayerEnvOverride","idTarget_ChangeMaterial","idTarget_ChangeColor","idTarget_Break","idTarget_Award_RushAttack","idTarget_Award_Adrenaline","idTarget_ApplyImpulse","idTarget_ApplyExplosionImpulse","idTarget_AnimWebUnpause","idTarget_AnimWebPause","idTarget_AnimWebChangeStateVia","idTarget_AnimWebChangeState","idTarget_AmplitudeTrigger","idTarget_AdaptiveTickToggle","idTarget_ActionScript","idTarget_AIProxy","idTarget_AIGlobalSettings","idTarget_AIEvent","idTargetSpawnGroup","idTarget","idSyncEntity","idSummoningTemplate","idStaticWaterEntity","idStaticVisibilityBlocker","idStaticMultiGuiEntity","idStaticEntity","idSpringCamera","idSplinePath","idSpitfireCannon","idSpectatorCamera","idSpectacleCamera","idSpawnPoint","idSpawnNodeSplines","idSpawnNode","idSpawnArea","idSoundTrigger","idSoundSubtitlesEntity","idSoundSphereEntity","idSoundPrefetchEntity","idSoundEntity","idSoundBoxEntity","idSoundBeamEntity","idSoundAreaEntity","idSlowMotionCamera","idRotatableCamera","idRiftBuildPosition","idRibbon2Emitter","idResurrectionProxy","idRegenArea","idReferenceMap","idPurifyingBeam","idProp_WeaponStatic","idProp_WeaponPickup","idProp_Usable","idProp_Static","idProp_Spawnable","idProp_PlayableDemonCircle","idProp_Pickup","idProp_OnlineCollectible","idProp_Moveable","idProp_HealthPickup","idProp_Explosive","idProp_ElectricArmor","idProp_DemonCircle","idProp_Coop_Billboard","idProp_Coop","idProp_ContinuallyUsed","idProp_BreakableLoot","idProp_Breakable","idProp_ArmorPickup","idProp_AIArmor","idProp2","idProp","idProjectile_SwarmMissile_V2","idProjectile_SwarmMissile","idProjectile_SiphonGrenade","idProjectile_RollingFire","idProjectile_Rocket","idProjectile_MeatHook","idProjectile_Mancubus_Smoke","idProjectile_GrenadeFast","idProjectile_Grenade","idProjectile_EMP","idProjectile_Destroyer","idProjectile_DamageOverTime","idProjectile_CyberdemonSwarmMissile","idProjectile_CacoDemonRocket","idProjectile_BfgBurn","idProjectile_BfgArc","idProjectile_Auger","idProjectile_AIHomingRocket","idProjectileShield","idProjectile","idPoseableEntity","idPortalWorld","idPortalSurface","idPlayerStart","idPlayer","idPieceEmitter","idPhotoModeCamera","idPerceptionVolume","idPathCorner","idParticleEmitter","idOverTheShoulderCamera","idObjective_Relay","idNetworkedParticleEmitter","idNetworkedEntityFx","idNetworkLight","idMusicEntity","idMultiplayerTrigger","idMoverModifier","idMover","idMidnightCutscene","idLowGravityMover","idLogicEntity","idLight","idLensFlare","idJostleSwivel","idJostleSpring","idJostleAnimated","idItemPropSpawner","idInventoryStorage","idInvasionBlocker","idInvasionAreaManager","idInteractionCamera","idInteractable_WorldCache","idInteractable_WeaponModBot","idInteractable_VegaTraining","idInteractable_Tutorial","idInteractable_SonicBoost","idInteractable_SlayerGate_Coop","idInteractable_SlayerGate","idInteractable_Shooter","idInteractable_Rune","idInteractable_Respec","idInteractable_Obstacle","idInteractable_NightmareMarker","idInteractable_Moveable","idInteractable_Minigame","idInteractable_LootDrop","idInteractable_LootCrate","idInteractable_JukeBox","idInteractable_HighlightSelector","idInteractable_GoreNest","idInteractable_GoreBud","idInteractable_GiveItems","idInteractable_EliteGuard_Coop","idInteractable_EliteGuard","idInteractable_Doom","idInteractable_CurrencyExchange","idInteractable_CollectibleViewer","idInteractable_Challenge_Shrine","idInteractable_BatterySocket","idInteractable_Automap","idInteractable","idInfo_UniversalTraversal","idInfo_Trigger_Facing_Target","idInfo_TraversalPoint","idInfo_TraversalChain","idInfo_TraversalBase","idInfo_TeleportDestination","idInfo_SpawnConditionProxy","idInfo_FastTravel","idInfo_DoorTraversalChain","idInfo_BounceDestination","idInfoWaitForButtonAfterLoad","idInfoTraversal_Ultimate","idInfoTraversal_Bot","idInfoTraversalEndPoint","idInfoTraversal","idInfoTexLod","idInfoStandPoint","idInfoSplineChild","idInfoSpawnPoint","idInfoRuler","idInfoRoam","idInfoPlayerHud","idInfoPath","idInfoOrbit","idInfoLookTargetGroup","idInfoLookTarget","idInfoLevelFadeIn","idInfoGoalPosition","idInfoFocus","idInfoFlightVolumeEntrance","idInfoExportHint","idInfoDebugMarker","idInfoCoverExposed","idInfoCover","idInfoCloudShot","idInfoBounds","idInfoAmbient","idInfoAAS","idInfo","idInfluenceSpawnSettings","idIOS_MeteorCrater","idHeightmapVolumeEntity","idGuiEntity_Text","idGuiEntity_Cinematic","idGuiEntity","idGoreEntity","idGladiatorShield","idGeomCacheEntity","idGameChallenge_Shell","idGameChallenge_PVPTutorial","idGameChallenge_PVP","idGameChallenge_CampaignSinglePlayer","idGameChallenge","idFuncSwing","idFuncShadowCaster","idFuncRotate","idFreeDbgCam","idFreeCamera","idFlightVolumeTraversalSpline","idExtraLifeTeleportLocation","idEscapePod","idEnvironmentalDamage_PointManager_Trigger","idEnvironmentalDamage_Point","idEnvironmentalDamage_Hurt_Trigger","idEnvArea","idEntityFxRandom","idEntityFx","idEntityCamera","idEntity","idEngineEntity","idEnergyShell","idEncounterVolume_ValidCover","idEncounterTrigger_RaiseUserFlag","idEncounterTrigger_PreCombat","idEncounterTrigger_OverrideCombatGrouping","idEncounterTrigger_Exit","idEncounterTrigger_Commit","idEncounterTrigger_AmbientAIAudio","idEncounterModifier_SetNextScriptIndex","idEncounterModifier_SetEnabledState","idEncounterModifier_Reset","idEncounterManager","idEncounterGroupMgr","idEncounterAmbientAudio","idElectricBoltEmitter","idEditorModelEntity","idDynamicVisibilityBlocker","idDynamicEntity_Damageable","idDynamicEntity","idDestructibleManager","idDestructible","idDestroyableProp","idDemonPlayer_Summoner","idDemonPlayer_Revenant","idDemonPlayer_Prowler","idDemonPlayer_Pinky","idDemonPlayer_PainElemental","idDemonPlayer_Marauder","idDemonPlayer_Mancubus","idDemonPlayer_Cacodemon","idDemonPlayer_Baron","idDemonPlayer_Archvile","idDemonPlayer_Arachnotron","idDemonPlayerStart","idDemonPlayer","idDecalEntity_Coop","idDecalEntity","idDebrisEntity","idDeathmatchPlayerStart","idDamageableAttachment","idCyberdemonMissileShooter","idCrusher","idCoopStartingInventory","idCombatVolume","idCombatGrouping","idCollisionStreamArea","idCollisionExclusionVolume","idCollectibleEntity","idCinematicCamera","idCaptureFrames","idCampaignInvaderStart","idCameraView","idCamera","idBuildArea","idBreakablePlatform","idBreakable","idBotPathNodeEntity","idBotActionEntity_Usable","idBotActionEntity_Info_RoamGoal","idBotActionEntity_Info_POI","idBotActionEntity_Info","idBotActionEntity","idBossInfo","idBloatedEntity","idBillboard","idBfgBurn","idBasePropSpawner","idBarricade","idBFG_Geyser","idAutomapSectionRevealTrigger","idAutomapMapGroupRevealEntity","idArchvileTemplate","idAnnouncementManager","idAnimated_ThreatSensor","idAnimated_AnimWeb_Samuel","idAnimated_AnimWeb","idAnimatedSimple_Faust","idAnimatedSimple_AnimSys","idAnimatedInteractable","idAnimatedEntity","idAnimatedAttachment_AF","idAnimatedAttachment","idAnimated","idAnimNode","idAnimCamera","idAmbientTriggerModifier_SetAITypes","idAmbientNodeGenerationArea","idAmbientNodeExclusionArea","idAlignedEntity","idActorWorkNode","idActorAmbientFilter","idActor","idActionNode","idAbnormalPlayerMetrics","idAITest_Rotation","idAITest","idAISearchHintGroup","idAISearchHintAuto","idAISearchHintAnimation","idAIMapMarkupEntity","idAIInteraction","idAIHintGroup","idAIHint","idAIDebugCamera","idAIDeathVolume","idAICombatHint_SuppressionFirePoint","idAICombatHint","idAIAASHint","idAI2","idAFEntity_GetUpTest","idAFEntity_Generic","idAFEntity_Dummy","idAFEntity_Corpse","idAASReachability","idAASObstacle"
};

static constexpr const char* g_all_types[] = 
#include "alltypes.h"
;
//#include "types_with_vtables.hpp"

struct enum_or_class_t {
	uintptr_t p;

	void set(classTypeInfo_t* cls) {
		p = ((uintptr_t)cls)|1;
	}
	void set(enumTypeInfo_t* enm ) {
		p = ((uintptr_t)enm);
	}

	bool isClass() const{
		return p&1;
	}

	bool isEnum() const{
		return !(p&1);
	}

	enumTypeInfo_t* getEnum() {
		if(!isEnum())return nullptr;
		return (enumTypeInfo_t*)p;
	}

	classTypeInfo_t* getClass() {
		if(!isClass())return nullptr;
		return (classTypeInfo_t*)(p & ~1ULL);
	}

};

struct type_node_t {
	enum_or_class_t m_typeinfo;

};

classTypeInfo_t* idType::FindClassInfo(const char* cname) {

	void* typeinfo_tools = *(void**)descan::g_global_typeinfo_tools;


	return reinterpret_cast<classTypeInfo_t* (*)(void*, const char*)>(descan::g_idtypeinfo_findclassinfo)(typeinfo_tools, cname);
		
}

enumTypeInfo_t* idType::FindEnumInfo(const char* ename) {


	void* typeinfo_tools = *(void**)descan::g_global_typeinfo_tools;



	return reinterpret_cast<enumTypeInfo_t* (*)(void*, const char*)>(descan::g_idtypeinfo_findenuminfo)(typeinfo_tools, ename);
}

static void dump1(std::string& filetext, const char* entname, std::set<std::string>& seen, std::vector<const char*>& queued) {

	auto ctype = idType::FindClassInfo(entname);

	if (!ctype)
		return;
	filetext += "\nstruct ";
	filetext += entname;
	if (ctype->superType && ctype->superType[0]) {
		filetext += " : public ";
		filetext += ctype->superType;

	}
	filetext += " {\n";

	for (auto bleh2 = ctype->variables; bleh2 && bleh2->name; ++bleh2) {
		//bleh2->
		filetext += "\t//";

		char buff[64];
		sprintf_s(buff, "Offset %d,\t size %d\n", bleh2->offset, bleh2->size);

		filetext += buff;
		if (bleh2->comment && bleh2->comment[0]) {
			filetext += "\t//";
			filetext += bleh2->comment;
			filetext += "\n";
		}
		filetext += "\t";
		filetext += bleh2->type;


		if (bleh2->ops) {
			filetext += bleh2->ops;
		}
		filetext += " ";
		filetext += bleh2->name;
		filetext += ";\n";
	}
	filetext += "};\n";
}

void idType::do_idlib_dump() {

	std::set<std::string> seen_names{};

	for (auto&& ename : g_all_entity_names)
		seen_names.insert(ename);

	std::string filetext{};
	std::vector<const char*> queued_types{};
	queued_types.reserve(131072);

	for (auto entname : g_all_types) {


		dump1(filetext, entname, seen_names, queued_types);


	}
#if 0
	while(queued_types.size() != 0) {
		std::vector<const char*> old_queued_types = std::move(queued_types);
		queued_types.reserve(65536);
		for(auto&& nq : old_queued_types) {
			dump1(filetext, nq, seen_names, queued_types);
		}
	}
#endif
	FILE* outtypes = nullptr;
	fopen_s(&outtypes, "eternal_idlib.h", "w");

	fputs(filetext.c_str(), outtypes);
	fclose(outtypes);

}

classVariableInfo_t* idType::FindClassField(const char* cname, const char* fieldname) {
	auto playerclass = idType::FindClassInfo(cname);
	if(!playerclass)
		return nullptr;
	auto headattr = playerclass->variables;
	classVariableInfo_t* notarget_field = nullptr;
	while(headattr && headattr->name) {
		if(!strcmp(headattr->name, fieldname)) {
			return headattr;
		}
		++headattr;
	}
	return nullptr;
}