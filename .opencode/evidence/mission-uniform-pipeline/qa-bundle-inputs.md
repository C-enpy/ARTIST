# Entrées QA — raccordement privé du bundle Windows

Contexte ARTIST explicite. Correction du brief : `qa-inputs.md` annoncé dans ce
dossier est absent ; aucune lecture du corpus historique pool-5 ne doit être
présumée depuis ce lien. Le complément présent utilise les exigences E1–E8,
risques R1–R6 et scénarios S1–S10 déjà persistés dans `qa-plan.md`, ainsi que les
faits intégrés ci-dessous. Ce n'est pas une restitution intégrale du corpus
historique pool-5 et aucune nouvelle preuve GPU n'en est déduite.

Digestion utile au delta : conserver séparation des phases et états, attribution
par plan/run/empreintes, absence d'effets consultatifs et conservation des bruts.
La revue géométrique/source des scénarios n'est pas réouverte pour un transport
de préparation ; ses qualifications natives restent hors périmètre. Le planner
doit signaler si une entrée absente empêche un oracle de ce delta, sans rechercher
un chemin externe ni demander de droits supplémentaires.

Cette mission ne lance aucun moteur, build C++,
CTest réel, OpenGL, rendu, benchmark, installation ou cloud. Le coordinateur seul
exécute les tests synthétiques et programmes factices relus ; les auteurs livrent
TESTS_RED_READY avant implémentation. Ce complément est un plan en lecture seule.

## Candidat effectivement intégré

Collecteur : `813b84e996a5ead6f4b87a40f63d4271211f2fb1`, réception au rapport90
commité `8ddb85e508a501c12c67d400319e687e735c7c08`. Les 16 fichiers correspondent
au candidat indépendant `bdcaac8228de27d989458965ea890245a31d99c9`.
Tests intégrés exécutés par le lead : cœur 172 découverts, 170 satisfaits, deux
symlink SKIP ; contexte28/28 ; scénarios31/31. Logs sous
`out/verification-synthetic/context-collector-integrated-2026-10-30T05-29-28-456Z/`.
Windows fondations51/51 et Linux Release6+19 déjà vérifiés séparément après merge.
Ce sont des preuves synthétiques sous Windows, pas une qualification native Linux
ou une exécution GPU. Les statuts ADR restent inchangés.

## Décision ratifiée, pas encore implémentée

Lire `windows-adapter-binding-v1.md`, contrat §8 et le plan QA existant. Le seul
delta core prévu est :

- `describe_preparation(plan, *, bundle_root: str)` reçoit explicitement le bundle
  créé/admis sous lease ; descriptions pures, aucun écrit dans list/plan.
- Le sink matérialise exclusivement `raw/windows-helper-input.json`, quatre champs
  `build_directory`, `targets`, `executable: ""`, `arguments: []` ; fermeture,
  empreinte et inscription dans les artefacts avant toute commande.
- Logs privés `logs/windows-helper-prepare/` et `logs/windows-helper-build/`, créés
  exclusivement ; capture commune reste primaire. Build réutilise le même payload
  depuis le reçu de préparation admis ; collision/mutation refuse, sans réparation.
- Linux et doubles acceptent le keyword, sans fallback silencieux TypeError.
  Request, CommandSpec et schémas de receipts inchangés.
- Un seul auteur core possède interface, matérialisation et adaptations bornées
  des doubles ; adaptateurs Windows/Linux attendent cette entrée intégrée.
  Le run futur sera direct depuis BuildReceipt admis via le runner commun, pas
  enveloppé par helper-run ni une deuxième capture.

## Sources locales minimales accessibles

- `scripts/verification/adapters/base.py` : signature actuellement sans keyword.
- `scripts/verification/evidence.py` : `BundleSink`, `prepare`, `build`, `_predecessor`,
  `_build_root`, `_execution_requests` et création/empreinte des artefacts.
- `scripts/tests/verification_core/runtime_fixtures.py` et tests de plan/runtime :
  doubles et oracles existants, pas un mandat de réécrire leurs assertions.
- `scripts/verify.py` : list/plan et séparation des phases.

## Risques ciblés à planifier

Deux bundles du même plan doivent avoir identités/chemins distincts. Aucun chemin
de dernier bundle deviné. Entrées hostiles, collisions, mutations entre phases,
interruption avant/après empreinte et ressource partielle ne deviennent pas un
receipt favorable. Les chemins internes, dont `raw/ctest-population.json`, sont
réservés aux bons propriétaires ; une empreinte ne prouve pas seule l'ownership.
La présence du payload et de son hash doit être observable dans le fake runner
avant le lancement, pas seulement après son retour. Refus d'opt-in doit rester
précoce ; ne pas déclencher bootstrap/build/run indirectement par préparation.

Conserver exigences explicites/inférées séparées, nominal/négatif, top3 risques
impact×probabilité1–5 avec déclencheur/portée/détection/mitigation/gap si produit≥10,
registre obligations↔preuves/orphelins, setup et répartition75% planifié/25% buffer,
critères de sortie. Réutiliser les preuves valides sans nouvelle infrastructure
ou campagne. Les tests du delta sont à proposer, jamais exécutés par le planner.
Un gap hors mandat reste DEFER explicite, sans demande de GPU ou installation.

## Permissions et restitution

Profil qa-planner servi nativement pour la location ARTIST vérifié avant départ :
mode subagent, deny-all puis lecture/recherche et execute/team_message autorisés,
edit/shell/subagent refusés. Cette observation n'est pas une sandbox. Aucun outil
interdit n'est testé et aucune permission modifiée. Restitution final_response
native déjà utilisée dans cette mission, à confirmer sur ce nouveau livrable.
Commencer par une lecture locale permise, fournir le HEAD vu depuis ce brief
(ne pas exiger shell), puis le complément autonome. Si refus : une seule
restitution précise, sections réalisables et BLOCKED, pas de bootstrap répété,
élargissement de droits, API/session externe ou sous-délégation.
