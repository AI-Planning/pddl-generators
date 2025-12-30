build:
	echo Skipping tidybot: installation complicated, fails to reliably generate solvable tasks

tidybot.jar:
	./lein compile
	./lein uberjar

	mv ./target/tidybot-1.0.1-snapshot-standalone.jar tidybot.jar
	rm -r ./target

test:
	@echo "[SKIP] skipping tidybot test: installation complicated, fails to reliably generate solvable tasks"

clean:
	rm -f tidybot.jar
