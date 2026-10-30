# 53 — Borner le harnais Windows avant son RED

## Remise et contrôle

La nouvelle tranche propose quinze tests du transport et du lancement Windows, avec environnement assaini, programme Python factice fini, identité des fichiers contrôlée et admission du seul fragment source revu par empreinte. Le coordinateur a lu les tests, le programme factice et le harnais. Cette admission par empreinte ne constitue pas une sandbox ni une autorisation d'exécuter ultérieurement du code GREEN non relu.

Le harnais contient encore des attentes sans délai maximal après arrêt du processus possédé et sur les lecteurs de sorties. Le watchdog du parent ne garantit pas à lui seul le nettoyage d'un enfant. Une correction de sûreté du seul harnais est demandée avant lancement : attentes bornées, diagnostic de précondition du harnais et nettoyage limité aux objets possédés. Les quinze tests, les quatre refus provisoires et les assertions antérieures restent inchangés.

## Portée de la prochaine admission

Le cas créant une jonction NTFS reste NON_EXECUTE pour cette admission ; il ne servira pas de remplacement aux tests de liens symboliques déjà refusés par Windows. Les quatorze autres méthodes seront sélectionnées explicitement après revue du correctif. Le test non lancé reste dans la suite, sans assertion affaiblie ; le gap reparse est conservé.

Les essais futurs ne lanceront que PowerShell comme harnais et l'enfant Python factice identifié, jamais le helper complet, MSVC ou un outil moteur. Les limites extrêmes de ligne de commande ne sont que construites, non lancées. Une panne d'environnement ou de nettoyage ne sera pas comptée comme RED de l'implémentation.

## État et preuves

Remise `result_mvudqe5b_000i_ngus25`, candidat `e2f7516ed89859328c5c832eac69452ac08952de`. Localisation : `native_fixture.ps1:170–171,185`, sous `scripts/tests/verification_windows/` du worktree auteur. Lecture de `NATIVE_RED.md`, `test_native_contracts.py` et `wrapper_child.py` également effectuée.

Aucun test de cette tranche exécuté. Les 37 tests Windows précédents restent une preuve de leur candidat antérieur seulement. Aucun moteur, compilation, rendu, benchmark, installation ou cloud lancé.
