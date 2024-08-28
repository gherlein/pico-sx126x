.PHONY: build flash


clean:
	-@rm -rf build
	-@rm *~ || true

git:
	git add *
	git commit -am"updated"
	git push origin main




