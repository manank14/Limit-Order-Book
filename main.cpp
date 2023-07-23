#include "LimitOrderBook.h"

int main(int argc, char* argv[]){
        LimitOrderBook limit_order_book("Reliance", 0.05, 2400, 2600);

        limit_order_book.new_quote(2500, enOrderSide::BUY, 10);
        limit_order_book.new_quote(2505, enOrderSide::BUY, 11);
        limit_order_book.new_quote(2520, enOrderSide::SELL, 10);
        limit_order_book.new_quote(2522, enOrderSide::SELL, 10);
        limit_order_book.print_lob(5);

        cout << endl;

        limit_order_book.cancel_quote(2);

        cout << endl;

        limit_order_book.print_lob(5);
}