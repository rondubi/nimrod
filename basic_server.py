from flask import Flask, redirect, url_for, request

app = Flask(__name__)

@app.route('/success/<name>')
def success(name): 
    return 'Server repeats back %s' % name

@app.route('/login', methods = ['POST', 'GET'])
def login(): 
    if request.method == 'POST': 
        user = request.form['nm']
        return redirect(url_for('success', name = user))
    else:
        user = request.args.get('nm')
        return redirect(url_for('success', name = user))

#@app.route('/hello/<name>')

#def gfg():
    #return 'geeksforgeeks'
# app.add_url_rule('/', 'g2g', gfg)

#def hello_name(name):
    #return 'Hello %s!' % name

if __name__ == '__main__': 
    app.run(port=8000)


