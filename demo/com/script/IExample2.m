%%
h = actxserver('IExample2.object');

h.Buffer = 'hello world';

display(h.Buffer);

h.delete;
