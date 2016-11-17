CFLAGS = -Wall
LDFLAGS = -lpthread -luuid -lrt

orders: orders.c transaction.c simulator.c queue.c share.c performance.c
	gcc orders.c transaction.c -o orders  $(LDFLAGS)
	gcc simulator.c transaction.c queue.c -o simulator  $(LDFLAGS)
	gcc share.c transaction.c queue.c -o share  $(LDFLAGS)
	gcc performance.c -o performance  $(LDFLAGS)

clean:
	rm orders
	rm simulator
	rm share
	rm performance

