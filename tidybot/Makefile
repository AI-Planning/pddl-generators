build:
	echo Skipping tidybot: installation complicated, fails to reliably generate solvable tasks

tidybot.jar:
	./lein compile
	./lein uberjar

	mv ./target/tidybot-1.0.1-snapshot-standalone.jar tidybot.jar
	rm -r ./target

#test: tidybot.jar
#	./gentidy.py 10 2 3 1 2 3 0

clean:
	rm -f tidybot.jar
