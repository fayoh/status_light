PYCLIENT=statuslight_client*whl
PYOBJECTS=statuslight_client/statuslight.py
POETRY=poetry

all: dist/*wheel
	echo $(PYCLIENT)

dist/*wheel: $(PYOBJECTS)
	$(POETRY) check
	$(POETRY) build -qn
	cp dist/$(PYCLIENT) ../..

test:
	$(POETRY) install
	$(POETRY) run py.test --cov=statuslight_client.statuslight statuslight_client
	$(POETRY) run coverage xml

clean:
	rm -rf dist
