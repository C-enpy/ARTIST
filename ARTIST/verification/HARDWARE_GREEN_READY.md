# Cible OpenGL — validations Python, GREEN_READY

## RED semantique attribue au runner

Le lead rapporte sur `1834af6f748bbb49eef7bfb227c2af04e923666f` : 12 nouveaux
tests, 51 echecs d'assertions/sous-cas, 0 error/skip, code 1. Les contre-exemples
atteignaient bien les `ValueError` non levees ; les 17 historiques etaient verts
et inchanges. Hashs stables. Journaux annonces :
`out/verification-synthetic/scenario-hardware-semantic-red-2026-10-29T23-59-52-502Z/`.
L'auteur n'a execute aucun test. GREEN des validations Python ensuite autorise.

## Candidat present

Seul `scripts/verification/scenarios/protocol.py` change pour le comportement :

- paire optionnelle complete ; nonchaine/vide refusee ; plafond inclusif 4096
  caracteres Unicode, pas octets ; controles C0/DEL/C1 et surrogates invalides
  refuses ; profil hardware uniquement ; sans paire, anciens argv identiques ;
- les deux attentes facultatives restent hors des neuf champs d'identite JSON
  et des quatre parameters. Le plan_sha256 est toujours compare exactement ;
  le core reste proprietaire de l'inclusion effective de la cible dans ce hash ;
- environment doit conserver exactement les vendor/renderer attendus et observes
  concordant avec le plan ; pas de sous-chaine ni normalisation du match ;
- refus des renderers logiciels connus meme si cible/observation concordent :
  llvmpipe, softpipe, SwiftShader, software, swrast et GDI Generic ;
- qualification MATCHED_EXPECTATION requise mais insuffisante seule : contexte
  courant bool strict, version observee >=3.3 et profile mask core non ambigu ;
- controles de cible executes aussi en FAIL. Un FAIL attribue conforme reste
  lisible ; une identite ou cible etrangere n'est jamais masquee par son verdict.

Le match attribue une cible dans un environnement maitrise ; il ne prouve pas
une acceleration physique universelle, une feature avancee ou une performance.
Le parseur en memoire ne cree pas de mesure ni de receipt : absence/precondition
doit rester traitee ABSENT/INCONCLUSIVE par le collecteur, avec code natif brut
et resultat conserve. Il refuse la presentation favorable d'observations
insuffisantes pour une cible explicitement attendue. Sans attente optionnelle,
le contrat de lecture historique est conserve ; cela ne qualifie pas hardware.
Le core doit donc toujours bloquer run hardware sans cible avant tout scenario.

## Runner et limites

Les 29 tests (12 cibles et 17 historiques) sont strictement inchanges.
Commande proposee au lead uniquement :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p "test_*.py" -v
```

Attendu : 29/29, aucun echec/error/skip, sans modification des assertions.
Non observe par l'auteur : executions auteur 0, GREEN/non-regression de cette
extension INCONCLUSIVE en attente du runner lead.

Aucun natif, description, core, adapter, modele ou test modifie. Les sources
natives restent celles de `60f32c5`, hardware fail-closed et adaptation cible
encore non autorisee avant GREEN observe. La future borne native sera aussi
4096 caracteres Unicode ; aucune compilation/execution C++ pour l'etablir ici.
Moteur, GL, rendu, benchmark, installation, cloud : NON_EXECUTE.
