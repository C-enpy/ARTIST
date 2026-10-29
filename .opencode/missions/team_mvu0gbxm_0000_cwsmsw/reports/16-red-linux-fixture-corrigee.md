# 16 — Fixture corrigee, RED Linux Release qualifie

Le candidat 0e6a0614dc77e3891e94155e627a923b02152949 modifie uniquement la normalisation du chemin de fixture par resolve, sans changer les assertions du helper ni les presets. Le lead a relu le diff puis rejoue les six controles synthetiques.

Observe : garde Debug verte, cinq echecs Release attendus, six tests executes, code1, aucune erreur d'import/environnement. Empreintes test/helper/presets/profil identiques avant/apres run. Le faux echec de casse de lecteur Windows est elimine sans modifier la production.

Le launcher Python du lead initialise tempfile.tempdir sur le repertoire temporaire utilisateur approuve avant le clear volontaire d'environnement dans les tests, puis appelle la meme decouverte unittest via runpy. La commande exacte est conservee dans le manifeste ; aucun parametrage de produit n'est change. L'auteur est charge de rendre ce setup temporaire portable dans la fixture, sans chemin utilisateur code en dur.

GREEN est autorise pour l'ajout borne de Release au helper et aux presets, host/build Conan coherents et behavior historique preserve. Aucune execution reelle des commandes de construction n'est permise ; tests sous doubles seulement. L'adaptateur Linux attend encore les interfaces communes integrees.

Preuves : out/verification-synthetic/linux-release-red-fixture-2026-10-29T23-13-12-017Z/. Auteur linux-pipeline ; runner lead, Python3.13.1 Windows. Cette preuve de scripts ne qualifie ni Linux, ni une compilation Release, ni GL/performance.
