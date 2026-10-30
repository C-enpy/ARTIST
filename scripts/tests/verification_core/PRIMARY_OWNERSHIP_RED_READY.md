# P1 — RED_READY, interruption entre étapes d'ownership primaire

Base : `258a9cbc5523846b42e99a75d6facfc80e19ed2c`.
Ré-audit communiqué par le lead `result_mvufdejj_0000_enwt6h` : R2 levé
statiquement, pipes/threads supprimés et propagation capture conforme, mais
ownership du primaire toujours bloquant dans les fenêtres extérieures au wait.
La preuve lead précédente «141 + 2 SKIP» reste attachée à ses cas/candidat ;
aucun rejeu R2, nouveau GREEN ou qualification native n'est revendiqué ici.

## Cause et test minimal

Le primaire est déjà acquis lorsque l'interruption se produit avant le try de
wait, ou entre son handler et la boucle stop/reap. Le handler extérieur actuel
enregistre alors l'interruption puis snapshot/close sans arrêter/reap le primaire.

`test_primary_ownership_interrupt.py` ajoute UNE méthode ciblant la fenêtre
juste après acquisition, avant le premier wait. Aucun code de production n'est modifié.
Le faux Popen écrit les deux préfixes dans les fichiers privés puis retourne un
objet primaire sans PID ni thread. Son wait valide chaque délai fini positif et
refuse plus de quatre appels ; kill/terminate ne modifient que des booléens/codes.

La cible de trace est calculée depuis l'AST du fichier effectivement importé :
classe/méthode, affectation child depuis popen_factory, première garde child
après acquisition. Vérification du
co_firstlineno et de la relation «garde avant premier wait». Aucun numéro de
ligne constant ni offset arbitraire. Le callback ne vise que le code exact de
SubprocessRunner.run et une ligne AST ; il vérifie que child est ce primaire et
que le reap n'est pas confirmé. L'injection est désarmée avant de lever UNE fois
KeyboardInterrupt. La trace antérieure est restaurée dans un finally même si
la fixture ou une assertion échoue.

Après acquisition, aucun wait initial n'a eu lieu lorsque l'injection arrive :
timed_out doit rester False. La fenêtre signalée entre handler et stop reste à
relire dans le correctif englobant ; le test ne dépend pas de l'emplacement de
la future boucle de cleanup et ne prétend pas reproduire toutes les interruptions.

Exiger stopped ET reaped avant retour, attente bornée,
interrupted=True, code natif exact et deux préfixes bruts disponibles (octets non
UTF-8 inclus). Si l'erreur privée transporte l'outcome, inspecter celui-ci plutôt
que provoquer un ERROR de test sans vérifier l'ownership. Les handles parent
doivent déjà être fermés avant le cleanup de secours propre au test.

RED attendu sur cette base : injection effectivement atteinte, mais stopped=False
et reaped=False. Une erreur de placement AST/fixture n'est pas un RED causal ;
ne pas déplacer une constante de ligne pour obtenir un échec artificiel. Les
assertions aval restent à atteindre après la correction, sans prétendre que ce
RED prouve déjà individuellement leur sensibilité.

## Confinement et invariants acquis

Popen réel est refusé par patch et le runner reçoit explicitement la factory.
Tous les temporaires sont dirigés sous le TemporaryDirectory possédé. Aucun
processus/descendant réel, signal OS, thread, sleep, réseau, moteur, GL, build,
benchmark, cloud, grant ou junction. Les seules mutations de sys.settrace sont
locales à l'expérience synchrone et restaurées ; l'injection n'est pas répétée.
Le cleanup de secours ferme les fichiers réellement possédés, sans masquer de
PermissionError. Une précondition filesystem défaillante reste une erreur
d'environnement à caractériser, pas une démonstration du défaut produit.

Les 143 méthodes, mappings et fixtures acquis restent byte-identiques. Nouveau
total statique144 ; aucun import candidat ou test exécuté par l'auteur. La garde
hardware, les adaptateurs et le collecteur sont inchangés. Aucun nouveau schéma,
transporteur d'erreur ou API n'est introduit. Correction privée seulement après
RED observé et autorisation GREEN du lead ; ré-audit après correction.

## Commande proposée au lead

Après revue de la fixture déterministe et de son nettoyage :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_primary_ownership_interrupt.py' -v
```

Attendu : une méthode, un échec pour l'absence de stop/reap, zéro
processus réel ; la commande n'a pas été exécutée par l'auteur. Le source seul
et AST/diff ne démontrent pas le comportement. Windows/Linux natif, GL et la
qualification de rendu restent NON_EXECUTE.
