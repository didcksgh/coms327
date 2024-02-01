Need to download the pokedex file. 
Your program should first look under
/share/cs327. Failing that, it should look under $HOME/.poke327/. And, optionally, it may look in
a third place (which is not under your source tree) of your choosing. For the second location, use getenv()
to resolve the value of the HOME environment variable.