/*
 * =====================================================================================
 *
 *       Filename: player.hpp
 *        Created: 04/08/2016 22:37:01
 *    Description: 
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */

#pragma once
#include <cstdint>
#include "monoserver.hpp"
#include "charobject.hpp"

class Player final: public CharObject
{
    protected:
        const uint32_t m_DBID;
        const uint32_t m_JobID;

    protected:
        uint32_t m_ChannID;

    protected:
        uint32_t m_Exp;
        uint32_t m_Level;

    protected:
        uint32_t m_Gold;
        std::vector<CommonItem> m_Inventory;

    public:
        Player(uint32_t,                // DBID 
                ServiceCore *,          //
                ServerMap *,            //
                int,                    // map x
                int,                    // map y
                int);                   // direction

    public:
        ~Player();

    protected:
        uint32_t Exp() const
        {
            return m_Exp;
        }

        uint32_t Gold() const
        {
            return m_Gold;
        }

        uint32_t Level() const
        {
            return m_Level;
        }

        uint32_t DBID() const
        {
            return m_DBID;
        }

        uint32_t JobID() const
        {
            return m_JobID;
        }

        uint32_t ChannID()
        {
            return m_ChannID;
        }

    public:
        int Speed()
        {
            return 5;
        }

        bool Update();

    protected:
        void OperateNet(uint8_t, const uint8_t *, size_t);

    protected:
        void OperateAM(const MessagePack &);

    private:
        void On_MPK_EXP(const MessagePack &);
        void On_MPK_MISS(const MessagePack &);
        void On_MPK_ACTION(const MessagePack &);
        void On_MPK_ATTACK(const MessagePack &);
        void On_MPK_OFFLINE(const MessagePack &);
        void On_MPK_CORECORD(const MessagePack &);
        void On_MPK_PICKUPOK(const MessagePack &);
        void On_MPK_UPDATEHP(const MessagePack &);
        void On_MPK_METRONOME(const MessagePack &);
        void On_MPK_MAPSWITCH(const MessagePack &);
        void On_MPK_NETPACKAGE(const MessagePack &);
        void On_MPK_BADCHANNEL(const MessagePack &);
        void On_MPK_NOTIFYDEAD(const MessagePack &);
        void On_MPK_NOTIFYNEWCO(const MessagePack &);
        void On_MPK_DEADFADEOUT(const MessagePack &);
        void On_MPK_BADACTORPOD(const MessagePack &);
        void On_MPK_BINDCHANNEL(const MessagePack &);
        void On_MPK_CHECKMASTER(const MessagePack &);
        void On_MPK_SHOWDROPITEM(const MessagePack &);
        void On_MPK_QUERYCORECORD(const MessagePack &);
        void On_MPK_QUERYLOCATION(const MessagePack &);
        void On_MPK_REMOVEGROUNDITEM(const MessagePack &);

    private:
        void Net_CM_REQUESTSPACEMOVE(uint8_t, const uint8_t *, size_t);
        void Net_CM_QUERYCORECORD   (uint8_t, const uint8_t *, size_t);
        void Net_CM_ACTION          (uint8_t, const uint8_t *, size_t);
        void Net_CM_PICKUP          (uint8_t, const uint8_t *, size_t);
        void Net_CM_QUERYGOLD       (uint8_t, const uint8_t *, size_t);

    private:
        void For_CheckTime();

    protected:
        void ReportGold();
        void ReportStand();
        void ReportHealth();
        void ReportDeadUID(uint64_t);
        void ReportCORecord(uint64_t);
        void ReportOffline(uint64_t, uint32_t);
        void ReportAction(uint64_t, const ActionNode &);

    protected:
        void DispatchOffline();

    protected:
        bool StruckDamage(const DamageNode &);

    protected:
        DamageNode GetAttackDamage(int);

    protected:
        bool DCValid(int, bool);
        bool InRange(int, int, int);

    protected:
        bool ActionValid(const ActionNode &);
        bool MotionValid(const ActionNode &);

    protected:
        void CheckFriend(uint64_t, const std::function<void(int)> &);

    protected:
        void OnCMActionMove  (CMAction);
        void OnCMActionStand (CMAction);
        void OnCMActionSpell (CMAction);
        void OnCMActionAttack(CMAction);
        void OnCMActionPickUp(CMAction);

    private:
        bool PostNetMessage(uint8_t, const uint8_t *, size_t);
        template<typename T> bool PostNetMessage(uint8_t nHC, T& stMessage)
        {
            return PostNetMessage(nHC, (const uint8_t *)(&stMessage), sizeof(stMessage));
        }

    protected:
        virtual bool GoDie();
        virtual bool GoGhost();

    protected:
        bool Offline();

    protected:
        virtual int MaxStep() const;

    protected:
        virtual void RecoverHealth();

    protected:
        uint32_t GetLevelExp();

    protected:
        bool DBUpdate(const char *, const char *, ...);
        bool DBAccess(const char *, const char *, std::function<std::string(const char *)>);

    protected:
        void GainExp(int);

    protected:
        void PullRectCO(int, int);

    protected:
        bool CanPickUp(uint32_t, uint32_t);

    protected:
        bool DBLoadPlayer();
        bool DBSavePlayer();

    protected:
        void CheckFriendType(uint64_t, std::function<void(int)>) override;
};
