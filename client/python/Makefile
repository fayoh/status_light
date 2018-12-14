PYCLIENT=statuslight_client*whl
PYOBJECTS=statuslight_client/statuslight-client.py
POETRY=poetry

all: dist/*wheel
	echo $(PYCLIENT)

dist/*wheel: $(PYOBJECTS)
	$(POETRY) build -qn
	cp dist/$(PYCLIENT) ../..

clean:
	rm -rf dist
