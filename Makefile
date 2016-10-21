CFLAGS = -Wall
LDFLAGS = -lpthread -luuid -lrt

orders: orders.c transaction.c simulator.c queue.c
	gcc orders.c transaction.c -o orders  $(LDFLAGS)
	gcc simulator.c transaction.c queue.c -o simulator  $(LDFLAGS)

clean:
	rm orders
	rm simulator

