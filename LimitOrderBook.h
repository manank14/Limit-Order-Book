#include <map>
#include <unordered_map>
#include <string>
#include <utility>
#include <list>
#include <iostream>

using namespace std;

typedef uint64_t Price; // Using Price * 100 to avoid decimals
typedef uint32_t Qty;

enum enOrderSide {
    BUY, 
    SELL
};

struct OrderInfo {
    OrderInfo(Price price, Qty qty, enOrderSide side, int oid) : m_Price(price), m_Qty(qty), m_Side(side), m_OID(oid){
    }
    Price m_Price;
    Qty m_Qty;
    enOrderSide m_Side;
    int m_OID;
    Qty m_FilledQty = 0;
};

class LimitOrderBook{
private:
    map<Price, pair<list<OrderInfo>, Qty>, greater<Price>> m_BidLOB; // Map of price -> pair <order list , net qty>
    map<Price, pair<list<OrderInfo>, Qty>> m_AskLOB; // Map of price -> pair <order list , net qty>

    unordered_map<int, pair<list<OrderInfo>::iterator, Price>> m_OrderIDMap; // Map of Oid -> pair <iterator of order , price>
    Price m_BestBid = 0;
    Price m_BestAsk = 0;

    Qty m_TotalBuyQty = 0;
    Qty m_TotalSellQty = 0;

    Price m_TickSize;
    Price m_UpperLimit;
    Price m_LowerLimit;

    string m_Underlying;

    int m_OIDCount = 0;

public:
    LimitOrderBook(string underlying, double tick_size, double lower_limit, double upper_limit) 
    : m_Underlying(underlying), m_TickSize(Price(tick_size * 100)), m_UpperLimit(Price(upper_limit*100)), m_LowerLimit(Price(lower_limit*100)) {
    }; 
    ~LimitOrderBook(){};

    double get_best_bid(){
        return m_BestBid / 100.0;
    }

    double get_best_ask(){
        return m_BestAsk / 100.0;
    }

    void print_lob(int levels){
        auto bid_it = m_BidLOB.begin();
        auto ask_it = m_AskLOB.begin();

        for (int i=0 ; i<levels ; i++){
            if (bid_it != m_BidLOB.end()) {
                auto price = (bid_it->first)/100.0;
                auto qty = (bid_it->second).second;

                cout << qty << " @ " << price;
                bid_it++;
            } else {
                cout << " 0 @ 0.0 "; 
            }
            cout << "     ";
            if (ask_it != m_AskLOB.end()) {
                auto price = (ask_it->first)/100.0;
                auto qty = (ask_it->second).second;

                cout << qty << " @ " << price;
                ask_it++;
            } else {
                cout << " 0 @ 0.0 "; 
            }
            cout << endl;
        }
    }

    bool new_quote(double price, enOrderSide side, Qty qty){
        Price px = Price(price * 100);

        if (px % m_TickSize != 0) {
            cout << "Invalid Price ; not multiple of tick size" << endl; 
            return false;
        } else if (px > m_UpperLimit || px < m_LowerLimit) {
            cout << "Price Not in Limits" << endl;
            return false;
        }


        int order_id = ++m_OIDCount;
        OrderInfo oinfo(px, qty, side, order_id);

        if (side == enOrderSide::BUY){
            m_BidLOB[px].first.push_back(oinfo);
            m_BidLOB[px].second += qty;
            m_OrderIDMap[order_id] = make_pair(--m_BidLOB[px].first.end(), px);
            if(px > m_BestBid)
                m_BestBid = px;
        } else if (side == enOrderSide::SELL){
            m_AskLOB[px].first.push_back(oinfo);
            m_AskLOB[px].second += qty;
            m_OrderIDMap[order_id] = make_pair(--m_AskLOB[px].first.end(), px);
            if (price < m_BestAsk)
                m_BestAsk = px;
        }else{
            cout << "Invalid Order Side" << endl;
            return false;
        }

        return true;
    }

    bool cancel_quote(int oid){
        if (m_OrderIDMap.find(oid) == m_OrderIDMap.end()) {
            cout << "Invalid Order ID for cancellation" << endl;
            return false;
        }

        Price price = m_OrderIDMap[oid].second;
        auto itr = m_OrderIDMap[oid].first;

        auto &oinfo = *itr;

        if (price <= m_BestBid){
            m_BidLOB[price].second -= oinfo.m_Qty;
            m_BidLOB[price].first.erase(itr);
            if (m_BidLOB[price].second == 0) {
                m_BidLOB.erase(price);
            }
        } else if (price >= m_BestAsk){
            m_AskLOB[price].second -= oinfo.m_Qty;
            m_AskLOB[price].first.erase(itr);
            if (m_AskLOB[price].second == 0) {
                m_AskLOB.erase(price);
            }
        }

        m_OrderIDMap.erase(m_OrderIDMap.find(oid));

        return true;
    }
};
