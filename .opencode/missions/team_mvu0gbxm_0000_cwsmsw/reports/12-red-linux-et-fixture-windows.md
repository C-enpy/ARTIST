# 12 — Linux Release : RED confirme, garde Debug a corriger dans la fixture

Le lead a relu puis execute les six tests synthetiques proposes par linux-pipeline au commit 2d45a6dac3e12a9c2bfa1cc80cd460130db63162. Les appels subprocess.run sont doubles, Popen/os.system/reseau pieges ; aucune commande Conan/CMake/CTest reelle n'est lancee. Le helper Python existant est importe puis appele uniquement sous ces doubles, dans une fixture possedee.

Resultat observe : 6 tests, 6 echecs, code1 ; sources comparees avant/apres inchangees. Cinq echecs correspondent aux fonctions Release absentes : option --configuration release et presets linux-release. Le sixieme n'est pas un RED utile du produit : le test Debug compare des chaines de chemins Windows dont seule la casse du lecteur differe (c: versus C: apres resolve).

Le lead a demande une correction test-only de normalisation des chemins avant toute implementation GREEN. La garde Debug devra passer au baseline et les cinq echecs Release rester causaux. Il a egalement demande que le repertoire temporaire soit capture avant le vidage volontaire d'environnement, pour ne pas basculer sur C:/tmp ; aucun nettoyage du parent non possede n'est autorise.

Le contrat du helper, ses presets et la logique de production ne doivent pas etre modifies pour masquer ce defaut de fixture. L'adaptateur Linux attend encore les interfaces communes : il s'agit d'une vraie dependance, pas d'un RED d'import a compter.

Preuve brute et empreintes : out/verification-synthetic/linux-release-red-2026-10-29T23-04-44-803Z/. Executant : lead, Python3.13.1 Windows. Commande : C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_linux -p test_release_contract.py -v.

Cette execution ne qualifie pas Linux, Release compile, OpenGL ou performance. Aucun moteur, build, rendu, benchmark ou cloud execute.
