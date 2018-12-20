PYCLIENT=statuslight_client*whl
PYOBJECTS=statuslight_client/statuslight.py
POETRY=poetry

.PHONY: all test clean

all: dist/*wheel
	echo $(PYCLIENT)

dist/*wheel: $(PYOBJECTS)
	$(POETRY) check
	$(POETRY) build -qn
	cp dist/$(PYCLIENT) ../..

test:
	$(POETRY) install
	$(POETRY) run py.test --cov-report xml --cov=statuslight_client.statuslight statuslight_client
	$(POETRY) run pycodestyle statuslight_client

clean:
	rm -rf dist
