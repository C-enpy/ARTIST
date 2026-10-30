# Revue indépendante finale des compléments G1–G4

Restitution native complète `result_mvuzl44v_0000_r21li8`, test-reviewer/pool-45,
session `ses_e731b3680ffeZKvjEYIx0r0zSS`. Revue lecture seule, aucun code, test,
mutation, Git ou programme exécuté par le reviewer ; aucun fichier modifié.
Code fourni par le coordinateur165aee5, HEAD documentaire18024a2. Comparaison des
empreintes fournie par le runner et non recalculée indépendamment par le reviewer.

## Avis reçu

« Aucun finding bloquant établi dans les tests examinés. » Les assertions présentent
une capacité discriminante concrète pour les scénarios ci-dessous. Aucun correctif
demandé et aucune nouvelle commande runner nécessaire sur ce candidat inchangé.
Livraison admissible pour qualification ultérieure, sans extrapolation de
fonctionnement démontré. Le coordinateur accepte cet avis dans cette portée,
pas comme certification exhaustive du pipeline ou de la qualification native.

## Obligations, scénarios discriminants et assertions lues

Toutes les mutations suivantes sont des expériences de pensée, non exécutées.

- G1, continuité : si build recrée le témoin ou adopte un autre bundle,
  test_g1_windows_helper_seam.py254–293 compare bundle/run/plan, octets/SHA/mtime,
  artefacts des deux vrais reçus et observations avant publication.
- G1, refus sans réparation : si build ignore témoin modifié/absent,
  :295–326 exige exception, describe_build non appelé, aucun lancement, reçu ou
  journal build et aucune réparation.
- G1, première cause/partiels : si configure continue après Conan17 ou si une
  anomalie de collecte masque la cause, :334–360 exige FAILED, première cause
  exacte, absence configure/témoin/exécutable, flux et SHA avant reçu conservés.
- G2, environnement effectif : si le runner conserve contexte périmé, réutilise
  request ID ou laisse un forçage, test_effective_windows_environment.py91–175,
  243–296 compare argv et env effectivement reçus à la factory à des attentes
  du plan/reçu indépendantes d'env_delta. Routes et deux phases CTest distinctes.
- G2, CPU et oracle : :298–348 exerce CPU et contre-exemples des six champs,
  forçages, cwd, captures, shell et argv ; défaut attendu explicitement rejeté.
- G3, admission : si helper-run devient admis ou un refus produit des effets,
  test_helper_functional.py279–325 couvre refus run/opt-in/flags/payload.
  test_wrappers.py212–245 structurel est complémentaire, pas preuve suffisante seul.
- G4, migration : si ordre/cible/code23/restauration divergent,
  test_dispatch.py35–102 vérifie commandes exactes, arrêt Conan23 et audit de
  données sans évaluation ; test_helper_functional.py327–432 conserve sélection,
  codes17/23 et restauration succès/échec/exception.
- G4, arguments : si vides/guillemets/antislashs/caractères spéciaux sont altérés,
  test_wrappers.py26–27,140–152 compare le round-trip du programme factice et code17.

Sens inverse : baselines G1 pins/AST/leases/répétition → validité de la couture,
pas build natif ; contre-exemples G2 → qualité d'oracle, nominaux → vraie composition
adapter/runner ; inventaire textuel de migration → traçabilité, pas preuve
comportementale. Aucun orphelin manifeste identifié dans les compléments examinés.

## Preuves consultées et réserves

- Exécutions attribuées au runner lead : Windows5+4+10 puis G1 6+4 sans failure,
  error ou skip ; pas d'addition aux suites historiques recouvrantes.
- Manifeste migration identifiant lead/candidata130773 et journal
  effective-and-admission.stderr.log1–10 consultés personnellement par le reviewer.
- Présence statique : assertions, vraie collecte BundleSink/evidence.prepare/build
  avant _finish et substitution bornée des ports mémoire lues.
- Inférence : capacité attendue d'arrêter les mutants décrits ; aucune mutation
  ou exécution supplémentaire par le reviewer.
- Audit AST et inventaire historique non certifiés exhaustivement. Hashes actuels
  fournis par le lead. Seeds G2 ne remplacent pas G1 ; refus Windows logiciel et
  garanties historiques ne sont pas nouvellement exécutés par cette revue.

Qualification native Windows/Linux, outils installés, moteur, rendu, benchmark,
GPU et performance NON_EXECUTE. Cinq skips symlink/reparse historiques, jonction06,
encodage cmd non ASCII et Dispose/writers hérités gardent leurs limites.
Restitution team_message et finale native reçues ; admission/routage recoupés
au rapport155, pas de preuve sandbox ou enforcement négatif prétendue.
