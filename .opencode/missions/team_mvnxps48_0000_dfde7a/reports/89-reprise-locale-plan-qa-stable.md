# 89 — Reprise locale autorisée et amendement QA stable

2026-10-26. L'opérateur demande de reprendre, de terminer sans nouvelles questions. Suite à la proposition87/88, le lead reprend la tranche locale simplifiée ; pas de nouveau chantier d'outillage, modèle/provider ou admission QA incompatible. La QA ci-dessous est préparée par le lead, **pas** attribuée à un planner indépendant. Les droits des profils ne sont pas élargis. Le contrat85, l'inventaire PM et les remises architecte restent les entrées adoptées.

## Contrat, ownership, risque et méthode

HEAD d'entrée `6618f1c` ; Reader ancien82bf52d, diffs étrangers préservés, rapport88 non encore commité. Lead seul écrivain Reader.hpp/ReaderTests.cpp et13 fichiers probes listés85 ; docs/rapports/preuves ensuite. Évaluation PM/LLM réutilisée : Reader C84/R80 ; retrait probes brut54→70/R56 ; le plancher développement est respecté. Les branches D/C auparavant parallèles sont séquencées localement pour supprimer l'overhead d'admission ; investigations indépendantes PM/architecte déjà parallélisées. Revue indépendante du diff final à organiser sans contexte auteur, aucune nouvelle enquête expérimentale.

## Exigences et oracles avant code

Explicites : fichier stable, portable, erreurs contrôlées et acceptation après succès ; aucun concurrent garanti ; retrait nominatif des surcouches après consommateurs ; préserver correctifs/fixtures/preuves/diffs étrangers. Inférées du code et du contrat adopté : mode texte/diagnostics compatibles, contexte vidé avant tentative, accumulation locale, fermeture vérifiée avant publication, catch I/O ciblé et non-remap des autres exceptions.

- R1 lecture : vide, court,4096,8192,12325, queue partielle. Oracles : égalité string exacte via vrais fichiers stables et contexte standard ; existants + caractérisations ajoutées avant refactor.
- R2 texte/NUL : espaces/tabulations/LF et NUL autour de frontières4096/8192 préservés dans string. Aucun contrôle GL après NUL revendiqué. Pas d'octets Control-Z ni d'identité CRLF disque exigée en mode texte.
- R3 publication : source préremplie remplacée au succès (y compris vide), vidée après ouverture impossible ; diagnostic exact d'ouverture conservé. Renforcer tests avant refactor ; ce sont des caractérisations si déjà verts.
- R4 ownership/erreurs : inspection indépendante de l'ordre lecture validée→clear résidu→close→vérification→setShaderCode, scope ifstream, catch ciblé ; aucun nouveau callback. Extraction/close authentiques et non-publication après close échoué restent des gaps d'exécution nommés. Pas de faux remplacement par throw/flags/probe.
- R5 isolation : garder quatre régressions de fixtures, leurs helpers/voisins ; tests conservés exécutés. Aucune suppression globale de répertoire temporaire/archives.
- R6 nettoyage : contrôle exact production/CMake/CI/scripts avant retrait des13 fichiers ; seuls consommateurs exploratoires internes et docs historiques attendus. Diff final nominatif et recherche résiduelle distinguant active/historique ; wrappers partagés conservés.

Pertes délibérées : deux injections et oracle Windows remove-au-setter retirés avec leur seam (85). Archives/commits et verdicts historiques restent inchangés. R4 conserve les obligations de production mais ne transforme pas ces retraits en preuve portable de catch/close.

## Top3 risques QA (I×P, distincts des scores PM)

1. Troncature/ancien contenu accepté :5×3=15. Déclencheur : modification Reader ; blast radius chargements shaders ; détection R1–R3 et consumers ; mitigation algorithme inchangé/caractérisations préalables ; gap : erreurs authentiques tardives non provoquées.
2. Affaiblissement d'oracles lors retrait :4×3=12. Déclencheur : trois tests supprimés ; blast radius catch/close ; détection revue diff+registre R4 ; mitigation contrôles production gardés/pertes documentées ; gap explicite non clos.
3. Suppression de correctifs/consommateurs :4×3=12. Déclencheur : nettoyage13 fichiers/instrumentation ; blast radius tests/build/docs actifs ; détection refs/diff/CTest ; mitigation liste positive/fixtures KEEP et aucun writer concurrent ; gap consommateurs externes `detail` non exclus.

## Cycle et sorties

1. **Avant refactor** : écrire seulement les caractérisations R1–R3 ; build unit via wrapper MSVC existant, découverte et sélection Reader figées puis CTest sériel/logs/JUnit. Ni vert ni environnement absent n'est RED. Tout changement comportemental nouveau exige RED causal ; sinon rester refactor pur.
2. Retirer callback/instrumentation et les13 fichiers, conserver l'algorithme. Diff/refs relus et candidat intégré localement avant consommateurs.
3. Build normal unit+component, découverte sélectionnée pré-run et CTest `--parallel 1 --no-tests=error`, logs/JUnit complets. Header consommé largement : suite normale complète justifiée ; pas de sweep seeds.
4. ASan sur nouveau candidat : build existant séparé et protocole établi. Sélection complète existante proportionnée au header/ownership/publication ; découverte=sélection=exécution,17 historiques chacun une fois, zérofailure/error/skip, diagnostics tardifs. Population réelle expliquée après migration, jamais555 hérité automatiquement.
5. Revue C++ indépendante fresh sur contrat/diff/preuves ; gap-check local séparé de cette indépendance. Corrections éventuelles : même ownership, TDD pertinent, intégration puis validation/ré-audit ciblés.
6. Documentation active et preuves portables, QMD après validation ; B1 revue finale et B5/C2 disposition acquise réconciliées sans nouvelle chasse. Sortie : bilan exact de chaque branche, pas PASS global si obligations authentiques restent sans preuve.

Ressources exclusives : index Git lead ; un build à la fois par sortie ; fixtures distinctes ; GL et ASan sériels réservés au runner lead. Enveloppe de travail :20% préparation,55% caractérisation/nettoyage/validation,25% buffer findings ; pas budget facturé ni promesse de durée. Commandes exactes et identités enregistrées avant chaque run dans les sorties `out/qa-f8/stable-20261026-01/` et logs dédiés. Aucun nouveau harness exécutable.

Registre bidirectionnel : R1–R3→JUnit pré/post et tests nommés ; R4→revue indépendante + gaps ; R5→tests fixture conservés ; R6→diff/résultats refs ; chaque preuve finale précise candidat/commande/population, toute preuve sans obligation est historique/orpheline à qualifier. Critères de sortie : checks choisis non nuls et verdicts recoupés, diff limité, archives/diffs étrangers préservés, limites explicitement consignées. Aucun GCC/Linux/Release/image/performance nouveau.

## Amendement documentaire après audit Q_STABLE natif — 2026-10-26

Le planner natif a ensuite audité ce plan a posteriori (rapport95), sans modifier
la qualité de son auteur initial ni antidater une QA indépendante. L'allocation
proposée ci-dessus20setup/55plan/25buffer correspond à68,75/31,25aprèssetup,
hors cible70–80/20–30. **Cible prospective corrigée :20setup/60plan/20buffer,
soit75/25aprèssetup** ; buffer pour investigationfinding,pannevalidation,réaudit.
Ce correctif n'est ni un tempsconsommé mesuré ni une réécriture du budget historique.
R4 reste présence statique/GAP d'exécution ; aucun GAP-accept ni preuve authentique
catch/close n'est créé par l'audit. R6 est désormais complété par contrôlesRO de
dispositionpool9/pool12, toujours bornés au corpus et sans garantie externe.
