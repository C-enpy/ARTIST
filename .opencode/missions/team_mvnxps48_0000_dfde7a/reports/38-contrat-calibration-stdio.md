# 38 — Contrat de calibration stdio, sans correction Reader

2026-10-26. G0 lead, décision34 + QA17 commitéfb3c9b9 + graphe37. **Contrat sonde adopté à cette portée**, contrat Reader toujours non adopté. Principal auteur cpp-engineer, recherche close engine-architect RO en parallèle ; build-engineer renfort seulement si vraie nouvelle panne toolchain. Pas de backend/installation/service/dépendance/seam supplémentaire.

## État et ownership

Source moteur82bf52d inchangée ; HEAD entrée decd225 puis commit intégrant37/38. Diffs concurrents non possédés. Auteur possède exclusivement scripts/probes/qa-f8-stdio/**, source/contrôles/protocole textuel ; aucun ARTIST/, CMakeLists, presets ou production Reader modifié. Lead possède sorties nouvelles out/qa-f8/stdio-<identité>, commandes/manifeste/logs/fixtures et exécution. Recherche close n'écrit aucun fichier. Vérifier HEAD/présence QA17/37/38 avant premier travail ; contenu non commité non transmis implicitement.

Adaptation confirmée utile : reprendre le mécanisme de verrou natif/témoin de l'ancienne sonde25, ajouter stdio et comparaisons modes manquantes. Source archivée connue out/qa-f8/calibration-20261024-01/f8-lock-probe.cpp, uniquement lecture/référence ; ne jamais écraser ancienne archive. Pas nouvelle interface moteur ni couche de politiques/templates génériques. Structure statique sonde ; chemins/octets/modes/erreurs/close au runtime, pas type par asset.

## Contrat observable

- M0 : fixtures possédées vide/court/4096/8192/12325/4608 et motifs CRLF/Ctrl-Z/BOM/NUL, chemins espaces/nonASCII selon encodage courant. Comparer observations stdio texte au comportement ifstream texte actuel caractérisé. **Ne pas imposer égalité binaire quand traduction texte native intervient** : oracle brut distinct d'oracle texte ; toute divergence documentée avant choix. Source shader LF nominale exacte. Pas de mutation des fixtures après sélection ni tolérance ad hoc.
- M1 : ouverture réussie, acquisition verrou exclusif via handle séparé, témoin ReadFile échoue33 sur plage recouvrante, puis observation stdio. Position et état initiaux contrôlés pour ne pas lire un buffer préchargé à la place de l'opération verrouillée. Capturer n, ferror, feof, errno avant cleanup et décrire ordre d'opérations. Erreur réelle distinguée EOF = résultat favorable ; erreur native authentique mais ferror perdu = option stdio défavorable, pas assertion assouplie. Open refusé/erreur témoin absente = expérience non discriminante, INCONCLUSIVE.
- M2 : déverrouillage puis nouvelle lecture exacte/recovery et close réussi ; cleanup des seules ressources possédées, voisins intacts. Acquisition exclusive, fixture initialement absente et échec cleanup observables. Aucun close double/handle rendu invalide artificiellement.
- Fichiers valide lecture seule et propriétaire unique local, close une fois ; mêmes règles cleanup sur erreur, diagnostic primaire conservé. Échec authentique close pas prétendu par simple cleanup/fclose observé réussi. Aucune publication Reader ou compilation GL touchée.
- M3 préfixe réellement acquis seulement après M1 et hypothèse nouvelle/protocole validé ; hors premier run. M4 catch ifstream et M5 close échoué restent obligations non couvertes. Recherche M5 RO bornée : dispositif authentique ou manque précis au choix opérateur ; aucune substitution/DEFER automatique.

## TDD et runner distinct

Première remise auteur **RO : protocole concret RED/nominal/oracles et commandes dérivables**, aucun code de sonde ou contrôle avant validation lead de la tranche. Trouver test causal de capacité manquante/comportement existant, jamais implémenter volontairement un mauvais comparateur ni failflag. Toute partie expérimentale sans RED pertinent est nommée caractérisation ; absence de test causal pour nouveau code contractuel bloque son implémentation jusqu'au traitement explicite, pas exception silencieuse au TDD.

Après protocole accepté : auteur écrit le test/contrat d'abord, lead persiste/intègre et exécute RED avec toolchain fonctionnelle ; auteur implémentation minimale ensuite, lead GREEN et non-régression. Même propriétaire de tranche, pas phases parallèles. Les résultats de l'expérience CRT ne sont pas le RED moteur et une issue défavorable ne se corrige pas pour obtenir du vert.

## Préconditions et commandes

Réutiliser toolchain existante, protocole25 et entrée compile_commands réelle. Aucun build/conan/pip/install implicite. Commandes exactes finales **à dériver et approuver avant exécution**, jamais annoncer une forme historique comme déjà validée pour nouvelle sonde. Un build par sortie ; arguments Debug et wrapper existants conservés sauf substitutions mécaniques justifiées source/sorties/link. L'auteur ne compile/exécute rien ; le lead runner seul conserve commande intégrale, codes, stdout/stderr, discovery/selection si applicable, source/configuration/binaire et SHA avant/après. Aucun GPU/ASan/CTest complet requis par seul probe ; réévaluer seulement sur nouveau diff pertinent.

Essais antérieurs :25 baseline/recovery4608 exacts, ReadFile33→ifstream eof/publicationvide/exit10, pas fread ni catch/close authentique. Ne pas rejouer à l'identique. Deux essais sans information nouvelle imposent hypothèse discriminante nouvelle ou INCONCLUSIVE. Résultats interprétés par obligation, pas PASS global.

## Sortie et décisions restantes

Livrable S intégré et logs runner accessibles avant D. Proposition modes/chemins/messages et éventuelle migration Reader retournent à décision distincte opérateur après V/C. Toute impossibilité/substitution d'obligation catch/close exige accord explicite. ADR inchangés ; aucune preuve GCC/Linux/Release/image/performance ni coût nul inféré.
